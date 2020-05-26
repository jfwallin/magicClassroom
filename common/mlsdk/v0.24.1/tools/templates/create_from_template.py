#!/usr/bin/env python
import argparse
import inspect
import logging
import os
import os.path
import shutil
import sys


def logger():
    return logging.getLogger(__name__)

def parse_args():
    """Parse command line arguments from sys.argv."""
    parser = argparse.ArgumentParser(
        description=inspect.getdoc(sys.modules[__name__]))

    parser.add_argument(
        '-t', '--templateDir', dest='templateDir', type=os.path.realpath, required=True,
        help='Absolute path, or path relative to the working directory, to the directory containing the template project to use.')

    parser.add_argument(
        '-n', '--name', dest='name', required=True,
        help='The name of the new project.')

    parser.add_argument(
        '-o', '--outputDir', dest='outputDir', type=os.path.realpath, required=True,
        help='Absolute path, or path relative to the working directory, to the directory in which to generate the new project.')

    return parser.parse_args()

def main():
    args = parse_args()

    if not os.path.isdir(args.templateDir):
        print(args.templateDir + " does not exist")
        exit(1)

    if not os.path.isfile(os.path.join(args.templateDir, 'template.json')):
        print(args.templateDir + " does not specify a valid template directory (no template.json file found).")
        exit(1)

    if os.path.isdir(args.outputDir):
        print(args.outputDir + " already exists")
        exit(1)

    # copy the template and remove the unneeded template.json file
    shutil.copytree(args.templateDir, args.outputDir)
    os.remove(os.path.join(args.outputDir, 'template.json'))

    mlsdkRoot = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    if not os.path.exists(os.path.join(mlsdkRoot, ".metadata", "sdk.manifest")): # if looks like an MLSDK root
        mlsdkRoot = None
    # rename files and replace variables in files if necessary
    for root, dirs, files in os.walk(args.outputDir):
        for file in files:
            filePath = os.path.join(root, file)

            # replace variables in files if necessary
            try:
                with open(filePath) as f:
                    newText = f.read().replace('${projectName}', args.name)
                    newText = newText.replace('${projectNameLower}', args.name.lower())

                    if mlsdkRoot:
                        newText = newText.replace('${mlsdkRoot}', mlsdkRoot)
                
                with open(filePath, "w") as f:
                    f.write(newText)

                if '${projectName}' in file:
                    newPath = filePath.replace('${projectName}', args.name)
                    os.rename(filePath, newPath)
                    filePath = newPath

                print(filePath)
            except UnicodeDecodeError:
                print("Skipping non-unicode file", filePath)

    print("New project generated in " + args.outputDir)

if __name__ == '__main__':
    main()
