import json 

"""

        private class sceneElement
        {
        public Vector3 position ;
        public Quaternion rotation ;
        public Vector3 scale ;

        public bool rigidbody ;
        public string name ;
        public string textureName;
        public string objectShape ;

        public float rotationRate;
        public float rotationPhase;
        public string objectOrbiting;
        public float orbitalRadius;
        public float orbitalPeriod;
        public float orbitalPhase;
        """
"""
        {
            "position": {
                "x": -0.570899248123169,
                "y": 2.188019275665283,
                "z": 1.2467663288116456
            },
            "rotation": {
                "x": 0.0,
                "y": 45.0,
                "z": 0.0,
                "w": 0.0
            },
            "scale": {
                "x": 0.6428495645523071,
                "y": 0.6428495645523071,
                "z": 0.6428495645523071
            },
            "rigidbody": true,
            "name": "ddd0",
            "textureName": "Jupiter",
            "objectShape": "Cube",
            "rotationRate": 0.0,
            "rotationPhase": 1.0,
            "objectOrbiting": "None",
            "orbitalRadius": 1.0,
            "orbitalPeriod": 1.0,
            "orbitalPhase": 0.0
        },
        """


"""
{u'rigidbody': True, u'scale': {u'y': 0.6164940595626831, u'x': 0.6164940595626831, u'z': 0.6164940595626831}, u'orbitalPhase': 0.0, u'name': u'ddd1', u'textureName': u'Jupiter', u'rotationRate': 0.0, u'orbitalPeriod': 1.0, u'rotationPhase': 1.0, u'objectOrbiting': u'None', u'position': {u'y': 2.3457250595092773, u'x': 0.12801051139831543, u'z': 1.9835379123687744}, u'rotation': {u'y': 45.0, u'x': 0.0, u'z': 0.0, u'w': 0.0}, u'objectShape': u'Cube', u'orbitalRadius': 1.0}
"""
sampleObject = {
        u'position': {u'x': 0.0, u'y': 0,0, u'z': 0.0}, 
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

with open("../jsondata/tmpobj.txt") as f:
    data = json.load(f)

#print data
print data["Items"][1]


