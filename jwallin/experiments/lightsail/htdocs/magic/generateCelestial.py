#!/usr/bin/python2.7
import json 
import copy
import re

class generateCelestial:

    def __init__(self):
        """
        self.sampleObject = {
                u'position': {u'x': 0.0, u'y': 0.0, u'z': 0.0}, 
                u'rotation': {u'x': 0.0, u'y': 0.0, u'z': 0.0, u'w': 0.0}, 
                u'scale': {u'x': 1.0, u'y': 1.0, u'z': 1.0}, 
                u'rigidbody': True,
                'orbitalPhase': 0.0, 
                u'name': u'Jupiter', 
                u'textureName': u'Jupiter', 
                u'objectShape': u'Sphere', 
                u'rotationRate': 0.0, 
                u'rotationPhase': 1.0, 
                u'objectOrbiting': u'None', 
                u'orbitalRadius': 1.0,
                u'orbitalPeriod': 1.0} 
                """
        self.sampleObject = {
                'position': {'x': 0.0, 'y': 0.0, 'z': 0.0}, 
                'rotation': {'x': 0.0, 'y': 0.0, 'z': 0.0, 'w': 0.0}, 
                'scale': {'x': 1.0, 'y': 1.0, 'z': 1.0}, 
                'rigidbody': True,
                'orbitalPhase': 0.0, 
                'name': 'Jupiter', 
                'textureName': 'Jupiter', 
                'objectShape': 'Sphere', 
                'rotationRate': 0.0, 
                'rotationPhase': 1.0, 
                'objectOrbiting': 'None', 
                'orbitalRadius': 1.0,
                'orbitalPeriod': 1.0} 

        
        with open("./moons.json") as f:
            self.moons = json.load(f)

        with open("./planets.json") as f:
            self.planets = json.load(f)

    def findPlanet(self, planetName):

        actualSize = 0.0
        actualRotation = 0.0
        actualOrbitalRadius = 0.0
        actualOrbitalPeriod = 0.0

        for p in self.planets:
            if p["name"] == planetName:
                actualSize = p["diameter"]
                actualRotation = p["rotationPeriod"]
                actualOrbitalRadius = p["distanceFromSun"] * 1e6
                actualOrbitalPeriod = p["orbitalPeriod"]
        return [actualSize, actualRotation, actualOrbitalRadius, 
                actualOrbitalPeriod ]

    def findMoon(self, moonName):

        actualSize = 0.0
        actualRotation = 0.0
        actualOrbitalRadius = 0.0
        actualOrbitalPeriod = 0.0

        for p in self.moons:
            if p["name"] == moonName:
                actualSize = p["radius"] * 2.0
                actualRotation = p["orbitalPeriod"]
                actualOrbitalRadius = p["distance"] * 1000.0
                actualOrbitalPeriod = p["orbitalPeriod"]
       

        return [actualSize, actualRotation, actualOrbitalRadius, 
                actualOrbitalPeriod ]

    def createMoon(self, name, position, sizeScale, rotationScale, orbitalScale, shape):

        [actualSize, 
            actualRotation, 
            actualOrbitalRadius, 
            actualOrbitalPeriod] = self.findMoon(name)
        
        p = self.sampleObject.copy()

        p['name'] = name

        p["position"]["x"] = position[0] + actualOrbitalRadius * orbitalScale
        p["position"]["y"] = position[1]
        p["position"]["z"] = position[2]

        p["rotation"]["x"] = 0.0
        p["rotation"]["y"] = 0.0
        p["rotation"]["z"] = 0.0
        p["rotation"]["w"] = 0.0

        scale = actualSize * sizeScale 
        #print "zzz", name, "size =",scale, "position=",actualOrbitalRadius * orbitalScale
        p["scale"]["x"] = scale
        p["scale"]["y"] = scale
        p["scale"]["z"] = scale

        p["rigidbody"] = False 
        
        p["orbitalPhase"] = 0.0
        p["textureName"] = name
        p["objectShape"] = shape
        
        p["rotationRate"] = actualRotation * rotationScale
        p["rotationPhase"] = 0.0
       
        p["objectOrbiting"] = "None"

        #print p
        #print "-------"
        return p.copy()
    
    def createPlanet(self, name, position, sizeScale, rotationScale, orbitalScale, shape):

        [actualSize, 
            actualRotation, 
            actualOrbitalRadius, 
            actualOrbitalPeriod] = self.findPlanet(name)
        
        p = self.sampleObject.copy()

        p['name'] = name

        p["position"]["x"] = position[0] 
        p["position"]["y"] = position[1]
        p["position"]["z"] = position[2]

        p["rotation"]["x"] = 0.0
        p["rotation"]["y"] = 0.0
        p["rotation"]["z"] = 0.0
        p["rotation"]["w"] = 0.0

        scale = actualSize * sizeScale 
        #print "zzz", name, "size =",scale, "position=",actualOrbitalRadius * orbitalScale
        p["scale"]["x"] = float(scale)
        p["scale"]["y"] = scale
        p["scale"]["z"] = scale

        p["rigidbody"] = False 
        
        p["orbitalPhase"] = 0.0
        p["textureName"] = name
        p["objectShape"] = shape
        
        p["rotationRate"] = actualRotation * rotationScale
        p["rotationPhase"] = 0.0
       
        p["objectOrbiting"] = "None"

        #print p
        #print "-------"
        return p.copy()




    def createSceneJson(self, sceneObjects):
        value = "{\"Items\": [" ;

        for ii in range(len(sceneObjects) -1):
            s = json.dumps(sceneObjects[ii],  indent=4, sort_keys=False)

            value = value + s + ", \n" 
        value = value + json.dumps(sceneObjects[-1], indent=4, sort_keys=False)
        value =  value + "]}";

        return value

    def createEarthMoon(self, sizeScale, rotationScale, orbitalScale, pheight, shape):


        # set the location of the primary body 
        position = [0.0, pheight,  0.0]

        # initialize the object list
        sceneObjects = []

        name = "Earth"
        orbitalScaleTmp = 0.0
        sceneObjects.append(gc.createPlanet(name, position, sizeScale, rotationScale, orbitalScaleTmp, shape))

        name = "Moon"
        sceneObjects.append(gc.createMoon(name, position, sizeScale, rotationScale, orbitalScale , shape))

        s1 = gc.createSceneJson(sceneObjects)
        return s1




    def createJupiter(self, sizeScale, moonSizeScale, rotationScale, orbitalScale, pheight, shape):


        # set the location of the primary body 
        position = [0.0, pheight,  0.0]

        # initialize the object list
        sceneObjects = []

        name = "Jupiter"
        orbitalScaleTmp = 0.0
        p1 = gc.createPlanet(name, position, sizeScale, rotationScale, orbitalScaleTmp, shape)
        sceneObjects.append(copy.deepcopy(p1))

        # add the moons with a relative orbital offset
        orbitalScale = projectedDistanceScale / actualEarthSize

        name = "Io"
        p1  = gc.createMoon(name, position, moonSizeScale, rotationScale, orbitalScale, shape)
        sceneObjects.append(copy.deepcopy(p1))

        name = "Europa"
        p1  = gc.createMoon(name, position, moonSizeScale, rotationScale, orbitalScale, shape)
        sceneObjects.append(copy.deepcopy(p1))

        name = "Callisto"
        p3  = gc.createMoon(name, position, moonSizeScale, rotationScale, orbitalScale, shape)
        sceneObjects.append(copy.deepcopy(p3))

        name = "Ganymede"
        p1  = gc.createMoon(name, position, moonSizeScale, rotationScale, orbitalScale, shape)
        sceneObjects.append(copy.deepcopy(p1))


        s1 = gc.createSceneJson(sceneObjects)
        return s1



gc = generateCelestial()

# initialize the scenes
scenes = []

# set up the scales for sizes and periods
projectedEarthSize = 0.2
actualEarthSize = 12756.0

projectedEarthRotation = 5.0
actualEarthRotation = 23.9

projectedDistanceScale = 0.02


sizeScale =  projectedEarthSize / actualEarthSize 
rotationScale =  projectedEarthRotation / actualEarthRotation
orbitalScale = projectedDistanceScale / actualEarthSize
pheight = 0.3
shape = "Sphere"

s1 = gc.createEarthMoon(sizeScale, rotationScale, orbitalScale, pheight, shape)
scenes.append(s1)



################### Scene 1  - Jupiter


with open("./jupiterParameters.json") as f:
    jupiterParameters= json.load(f)

actualEarthSize = 12756.0
actualEarthRotation = 23.9

projectedEarthSize = 0.07
projectedEarthRotation = 5.0
projectedDistanceScale = 0.02
moonScale = 4
shape = "Cube"


##print jupiterParameters
projectedEarthSize = float( jupiterParameters['projectedEarthSize'] )
projectedEarthRotation = float( jupiterParameters['projectedEarthRotation'] )
projectedDistanceScale = float( jupiterParameters['projectedDistanceScale'] )
moonScale = float( jupiterParameters['moonScale'] )
shape = jupiterParameters['shape'] 

sizeScale =  projectedEarthSize / actualEarthSize 
rotationScale = projectedEarthRotation / actualEarthRotation

moonSizeScale = sizeScale * moonScale



s1 = gc.createJupiter(sizeScale, moonSizeScale,  rotationScale, orbitalScale, pheight, shape)
scenes.append(s1)

sss = s1
#sss = str(s1).replace("\"","\\\"")
sss = sss.replace("\n","")
sss = re.sub(",\s+",",",sss) 
sss = re.sub("}\s+","}",sss) 
sss = re.sub("{\s+","{",sss) 
sss = re.sub(":\s+",":",sss) 
#sss = re.sub("\\","",sss)
#sss = re.sub("\{\s+","\{",sss) 
#sss = re.sub("\}\s+","\}",sss) 
#sss = re.sub("\s+\}","\}",sss) 
#sss = re.sub("\:\s+","\:",sss) 
print sss


