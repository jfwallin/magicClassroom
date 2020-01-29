# %BANNER_BEGIN%
# ---------------------------------------------------------------------
# %COPYRIGHT_BEGIN%
#
# Copyright (c) 2019 Magic Leap, Inc. (COMPANY) All Rights Reserved.
# Magic Leap, Inc. Confidential and Proprietary
#
#  NOTICE:  All information contained herein is, and remains the property
#  of COMPANY. The intellectual and technical concepts contained herein
#  are proprietary to COMPANY and may be covered by U.S. and Foreign
#  Patents, patents in process, and are protected by trade secret or
#  copyright law.  Dissemination of this information or reproduction of
#  this material is strictly forbidden unless prior written permission is
#  obtained from COMPANY.  Access to the source code contained herein is
#  hereby forbidden to anyone except current COMPANY employees, managers
#  or contractors who have executed Confidentiality and Non-disclosure
#  agreements explicitly covering such access.
#
#  The copyright notice above does not evidence any actual or intended
#  publication or disclosure  of  this source code, which includes
#  information that is confidential and/or proprietary, and is a trade
#  secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION,
#  PUBLIC  PERFORMANCE, OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS
#  SOURCE CODE  WITHOUT THE EXPRESS WRITTEN CONSENT OF COMPANY IS
#  STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE LAWS AND
#  INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE
#  CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS
#  TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE,
#  USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.
#
# %COPYRIGHT_END%
# ---------------------------------------------------------------------
# %BANNER_END%

import argparse
import json
import shutil
import os
import pathlib
import zipfile
import sys
import tempfile

def readSessionData(inputFile):
    # read the data from the original session file
    with open(inputFile) as json_file:
        data = json.load(json_file)
        return data

def copyAssetsAndUpdatePaths(jsonData, outputDirectory):
    # parse the root path in the session
    rootPath = jsonData['RootPath']

    # parse the top level assets (typically room files)
    for media in jsonData['Media']:
        if not media.get('RootID'):
          fileName = os.path.split(media['Path'])[1]
          # copy the top level asset file to the export archive folder
          sourcePath = os.path.join(rootPath, media['Path'])
          targetPath = os.path.join(outputDirectory, fileName)
          shutil.copy2(sourcePath, targetPath) # complete target filename given
          # update the path to use the local file path
          media['Path'] = "./" + fileName

    # update the root path to be the local file path
    jsonData['RootPath'] = "./"
    return jsonData

def saveRoomInfo(jsonData, outputDirectory):
    # save the session to the RoomInfo.json
    roomFile = os.path.join(outputDirectory, "RoomInfo.json")
    with open(roomFile, 'w') as roomInfoOutfile:
        json.dump(jsonData, roomInfoOutfile)

def zipRoomArchive(outputDirectory, outputFile):
    # initializing empty file paths list
    file_paths = []

    # crawling through directory and subdirectories
    for root, directories, files in os.walk(outputDirectory):
        for filename in files:
            # join the two strings in order to form the full filepath.
            filepath = os.path.join(root, filename)
            file_paths.append(filepath)


    # printing the list of all files to be zipped
    print('Following files will be zipped:')
    for file_name in file_paths:
        print(file_name)

    # writing files to a zipfile
    with zipfile.ZipFile(outputFile,'w') as zip:
        # writing each file one by one
        for file in file_paths:
            zip.write(file, os.path.split(file)[1])

    print('All files zipped successfully! Room archive %s created' % (outputFile))

def removeTempOutputDirectory(tempDir):
    # removing the temporary directory
    shutil.rmtree(tempDir, ignore_errors=True)

def main():
    parser = argparse.ArgumentParser(
        add_help=True,
        description='Session Packager - Handles packing .session files and the associated'
                    'asset files into a .room archive that can be used directly within the'
                    'Virtual Room Simulator of the Zero Iteration module of The Lab')

    parser.add_argument('inputFile', type=str, help='path to a .session file to package a .session extension will be appended if not given')
    parser.add_argument('outputFile', type=str, help='path to a .room file to use as the target archive to package the session assets into a .room extension will be appended if not given')

    args = parser.parse_args()
    inputFile = args.inputFile
    outputFile = args.outputFile

    # add a .session extension if one is not provided
    if pathlib.Path(inputFile).suffix == '':
        inputFile = inputFile + ".session";

    # verify there is a .session extension
    if pathlib.Path(inputFile).suffix != '.session':
        print('Invalid input session file to package - file is not a session file', file=sys.stderr)
        sys.exit(1)

    # verify the file exists and is a file
    if not os.path.exists(inputFile):
        print('Input session file %s does not exist, trying local path' % (inputFile), file=sys.stderr)
        inputFile = os.path.join(os.getcwd(), inputFile)
        if not os.path.exists(inputFile):
            print('Invalid input session file to package - file %s does not exist' % (inputFile), file=sys.stderr)
            sys.exit(1)

    if not os.path.isfile(inputFile):
        print('Invalid input session file to package - path is not a file', file=sys.stderr)
        sys.exit(1)

    # add a .room extension if one is not provided
    if pathlib.Path(outputFile).suffix == '':
        outputFile = outputFile + ".room";

    # verify there is a .room extension
    if pathlib.Path(outputFile).suffix != '.room':
        print('Invalid output room archive to package session into - file is not a room archive', file=sys.stderr)
        sys.exit(1)

    # get and verify the output file directory
    if os.path.dirname(outputFile) == '':
        outputFile = os.path.join(os.getcwd(), outputFile)

    # verify the file doesn't yet exist
    if os.path.exists(outputFile):
        print('Invalid output room archive to package session into - file already exists', file=sys.stderr)
        sys.exit(1)

    print('Packaging session %s into room archive %s' % (inputFile, outputFile))

    # create a temporary directory
    tempDirectory = tempfile.mkdtemp()

    try:
        data = readSessionData(inputFile)
        data = copyAssetsAndUpdatePaths(data, tempDirectory)
        saveRoomInfo(data, tempDirectory)
        zipRoomArchive(tempDirectory, outputFile)
    finally:
        removeTempOutputDirectory(tempDirectory)


if __name__ == "__main__":
    main()
