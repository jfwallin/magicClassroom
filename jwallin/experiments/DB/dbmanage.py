import mysql.connector

class dbmanage:

    def __init__(self):

        self.servername = "localhost"
        self.username = "myuser"
        self.password = "mysqlUsernamePassword"
        self.db = "duckduck"
        self.verbose = 0
        self.execute = 1
        self.olddb = 1


    def defineTables(self):

        self.table = [];

        self.table.append( ["Log", [
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY", "I","long"],
                ["macid","VARCHAR(30) NOT NULL","macid","string"],
                ["ipaddress", "VARCHAR(30) NOT NULL","ip","string"],
                ["devicetime", "BIGINT UNSIGNED","int","long"],
                ["image1", "VARCHAR(30) NOT NULL","name","string"],
                ["image1x", "DOUBLE","float","float"],
                ["image1y", "DOUBLE","float","float"],
                ["image1z", "DOUBLE","float","float"],
                ["image2", "VARCHAR(30) NOT NULL","name","string"],
                ["image2x", "DOUBLE","float","float"],
                ["image2y", "DOUBLE","float","float"],
                ["image2z", "DOUBLE","float","float"],
                ["servertime", "BIGINT UNSIGNED","I","long"],
                ["reg_date", "TIMESTAMP","I","string"]
                ]])

        self.table.append( ["Devices", [
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY","I","long"],
                ["macid","VARCHAR(30) NOT NULL","macid","string"],
                ["ipaddress", "VARCHAR(30) NOT NULL","ip","string"],
                ["school","INT UNSIGNED","name","string"], 
                ["reg_date", "TIMESTAMP","I","string"]
                ]])

        self.table.append(["Schools", [
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY","I","long"],
                ["name","VARCHAR(100) NOT NULL","name","string"],
                ["city", "VARCHAR(50) NOT NULL","name","string"],
                ["state","VARCHAR(50)","name","string"], 
                ["country","VARCHAR(50)","name","string"], 
                ["reg_date", "TIMESTAMP","I","string"]
                ]])

        self.table.append(["Sessions", [
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
                ["macid","VARCHAR(30) NOT NULL"],
                ["ipaddress", "VARCHAR(30) NOT NULL"],
                ["connecttime", "TIMESTAMP"] 
                ]])

        self.table.append(["Courses", [
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
                ["school", "INT UNSIGNED"],
                ["fullname","VARCHAR(100) NOT NULL"],
                ["catalogdesignation", "VARCHAR(50) NOT NULL"],
                ["subject","INT UNSIGNED"]
                ]])

        self.table.append(["Subject",[
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
                ["name","VARCHAR(100) NOT NULL"]
                ]])

        self.table.append(["ClassOfferings",[
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
                ["course", "INT UNSIGNED"],
                ["instructor", "INT UNSIGNED"],
                ["startdate", "TIMESTAMP"],
                ["enddate", "TIMESTAMP"],
                ["schedule", "VARCHAR(100)"],
                ["reg_date", "TIMESTAMP"]
                ]])

        self.table.append(["Students",[
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
                ["schoolid", "VARCHAR (100)"],
                ["lastname","VARCHAR (100) NOT NULL"],
                ["firstname", "VARCHAR (100) NOT NULL"],
                ["reg_date", "TIMESTAMP"]
                ]])

        self.table.append(["Instructors",[
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
                ["schoolid", "VARCHAR (100)"],
                ["lastname","VARCHAR (100) NOT NULL"],
                ["firstname", "VARCHAR (100) NOT NULL"],
                ["reg_date", "TIMESTAMP"]
                ]])

        self.table.append(["Lessons",[
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
                ["name", "VARCHAR (100) NOT NULL"],
                ["description", "VARCHAR (1000) NOT NULL"],
                ["exerciselist",  "VARCHAR (10000) NOT NULL"],
                ["reg_date", "TIMESTAMP"]
                ]])

        self.table.append(["Exercises",[
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
                ["exercise", "VARCHAR (100) NOT NULL"],
                ["description", "VARCHAR (1000) NOT NULL"],
                ["lastname","VARCHAR (100) NOT NULL"],
                ["firstname", "VARCHAR (100) NOT NULL"],
                ["reg_date", "TIMESTAMP"],
                ]])

        self.table.append(["ExerciseElements",[
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
                ["exercise","INT"],
                ["elementnumber","INT UNSIGNED"],
                ["elementtype", "INT UNSIGNED"],
                ["timestart", "INT"],
                ["reg_date", "TIMESTAMP"],
                ]])

        self.table.append(["Grades",[
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
                ["lesson","INT UNSIGNED"],
                ["exercise","INT UNSIGNED"],
                ["evaluation","INT UNSIGNED"],
                ["score", "INT UNSIGNED"],
                ["reg_date", "TIMESTAMP"]
                ]])

        self.table.append(["Objects",[
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
                ["name", "VARCHAR (100) NOT NULL"],
                ["object","JSON"],
                ["reg_date", "TIMESTAMP"]
                ]])

        self.table.append(["Evaluations", [
                ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
                ["maxscore", "INT UNSIGNED"],
                ["reg_date", "TIMESTAMP"]
                ]])

    def createDB2(self):
        sql = "SHOW DATABASES"
        if self.verbose == 1:
            print(sql)

        if self.execute == 1:
            self.mycursor.execute(sql)

        found = 0
        for x in self.mycursor:
            #print( "DB = " + x[0])
            if x[0] == self.db:
                found = 1

        #print("previous db state  = "+ str(found))
        if found == 1:
            sql = "DROP DATABASE " + self.db
            if self.verbose == 1:
                print(sql)
            if self.execute == 1:
                self.mycursor.execute(sql)

        sql = "CREATE DATABASE " + self.db
        if self.verbose == 1:
            print(sql)
        if self.execute == 1:
            self.mycursor.execute(sql)
    

    def createDB(self):
        sql = "CREATE DATABASE " + self.db
        if self.verbose == 1:
            print(sql)
        if self.execute == 1:
            self.mycursor.execute(sql)
        
    def connectDB(self):
        self.mydb = mysql.connector.connect(
            host=self.servername,
            user=self.username,
            passwd=self.password,
            auth_plugin="mysql_native_password",
            #database=self.db
            )
    
        self.mycursor = self.mydb.cursor()

    def dropDB2(self):
        self.mycursor.execute("SHOW DATABASES")
        found = 0
        for x in self.mycursor:
            if x[0]== self.db:
                found = 1

        if found == 1:
            sql = "DROP DATABASE " + self.db
            if self.verbose == 1:
                print(sql)
            if self.execute == 1:
                self.mycursor.execute(sql)

    def dropDB(self):
        sql = "DROP DATABASE " + self.db
        if self.verbose == 1:
            print(sql)
        if self.execute == 1:
            self.mycursor.execute(sql)

            
    def useDB(self):
        sql = "USE " + self.db
        if self.verbose == 1:
            print(sql)
        if self.execute == 1:
            self.mycursor.execute(sql)
    
    def listDB(self):
        self.mycursor.execute("SHOW DATABASES")
        if self.verbose == 1:
            for x in self.mycursor:
                print(x)

    def createAllTables(self):
        # construct the SQL to make the tables
        lt = len(self.table)
        for i in range(lt):
            tname = self.table[i][0]
            sql = "CREATE TABLE " + tname + " (\n"
            ncol = len(self.table[i][1])
            for j in range(ncol):
                column = self.table[i][1][j]
                cname = column[0]
                cdesc = column[1]
                if j < ncol-1:
                    sql = sql + cname + " " + cdesc + ",\n"
                else:
                    sql = sql + cname + " " + cdesc + "\n"
            sql = sql + ")"
            if self.verbose == 1:
                print (sql)
                print ("")
            if self.execute == 1:
                self.mycursor.execute(sql)

    def createTable(self, itable):
        # construct the SQL to make the tables
        lt = len(self.table)
        i = itable
        tname = self.table[i][0]
        sql = "CREATE TABLE " + tname + " (\n"
        ncol = len(self.table[i][1])
        for j in range(ncol):
            column = self.table[i][1][j]
            cname = column[0]
            cdesc = column[1]
            if j < ncol-1:
                sql = sql + cname + " " + cdesc + ",\n"
            else:
                sql = sql + cname + " " + cdesc + "\n"
        sql = sql + ")"
        if self.verbose == 1:
            print (sql)
            print ("")
        if self.execute == 1:
            self.mycursor.execute(sql)

    def dropAllTables(self):
        lt = len(self.table)
        for i in range(lt):
            self.dropTable(i)
            

    def dropTable(self, itable):
        # construct the SQL to make the tables
        i = itable
        tname = self.table[i][0]
        sql = "DROP TABLE " + tname 
        if self.verbose == 1:
            print (sql)
            print ("")
        if self.execute == 1:
            self.mycursor.execute(sql)

    def listTables(self):
       self.mycursor.execute("SHOW TABLES FROM " + self.db)
       for x in self.mycursor:
            print(x)

   
    def createPhpDefinition(self, itable):

        lt = len(self.table)
        i = itable
        tname = self.table[i][0]
        ncol = len(self.table[i][1])
        myarray = "$data[\""+ tname +"\"] = array(\n";
        for j in range(ncol):
            column = self.table[i][1][j]
            cname = column[0]
            cdesc = column[1]
            cvalidation = column[2]
            cstring = column[3]
            myarray = myarray + "  array(\"" + cname + "\", \"" + cvalidation + "\", \"" + cstring + "\", \"\", \"\")"
            if j < ncol-1:
                myarray = myarray + ","
            myarray = myarray + "\n"
        myarray = myarray + ");\n\n"
            

        if self.verbose == 0:
            #print (self.table[i][0])
            print (myarray)
        #if self.execute == 1:
        #    self.mycursor.execute(sql)

        return myarray

   
    def createCsharpObject(self, itable):

        lt = len(self.table)
        i = itable
        tname = self.table[i][0]
        ncol = len(self.table[i][1])
        myobject = "public class "+ tname +"\n{\n";
        for j in range(ncol):
            column = self.table[i][1][j]
            cname = column[0]
            cdesc = column[1]
            cvalidation = column[2]
            cstring = column[3]
            nd = cdesc
            


            nd = cstring
            myobject = myobject + "  public " + nd + " " + cname + ";\n";
#            myobject = myobject + "  array(\"" + cname + "\", \"" + cvalidation + "\", \"" + cstring + "\", \"\", \"\")"
#            if j < ncol-1:
#                myobject = myobject + ","
#            myobject = myobject + "\n"
        myobject = myobject + "};\n\n"
            

        if self.verbose == 0:
            #print (self.table[i][0])
            print (myobject)
        #if self.execute == 1:
        #    self.mycursor.execute(sql)

        return myobject



if __name__  == "__main__":

    db = dbmanage()

    olddb = db.olddb 
    if olddb == 1:

        # connects and lists DB
        db.connectDB()
        #db.listDB()

        # wipes out the old DB and reestablishs it
        db.createDB2()
        #db.listDB()

        # defines the tables and sets a single table
        db.useDB()
        db.defineTables()

        phpstring = "<?php \n\n"
        objectstring = "";
        for jj in range(3):
            db.createTable(jj)
        #    db.dropTable(0)
            myarray = db.createPhpDefinition(jj)
            myobject = db.createCsharpObject(jj)
            phpstring = phpstring + myarray
            objectstring = objectstring + myobject
        #db.listTables()

        phpstring = phpstring + "\n?>";\
        print "--------"
        print phpstring
        print "--------"
        print objectstring
        print "--------"
    #    db.dropDB2()

    if olddb == 0:
        db.createDB()
        db.defineTables()
        db.createTable(0)
        db.listTables()

        db.dropTable(0)
        db.dropDB()


#dbname = "myDB"
#dbAction = ["SELECT", "CREATE", "DROP"]
#taction = 1

"""
sql = "CREATE DATABASE myDB"

$dbAction = array("SELECT", "CREATE", "DROP");
$sql = $dbAction[$createTable] . " TABLE " . $tablename . " (
id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
macid VARCHAR(30) NOT NULL,
ipaddress VARCHAR(30) NOT NULL,
school VARCHAR(50),
reg_date TIMESTAMP
)";
"""
# mycursor.execute("CREATE TABLE customers (name VARCHAR(255), address VARCHAR(255))")


#for i in range(lt):
#    print table[i][0]

# sample queries



#Devices
# App startup
# query - does device exist in the database
# add - device to the database

#Schools
# schools are selected when a device is added
# schools are createdd when a device is added at a new site

#Courses
# courses are selected when a instructor creates a class offering
# courses are added when a course does not exist as an offering

#Subject
# subjects are selected when a instructor creates a course
# subjects are added when a instructor selects a course - or when admin authorizes it

#ClassOfferings
# class offerings are created by instructors
# they use the schools and courses only
# they have a time, instructor and place

#Students
# students are uploaded by the instructor in a list
# students can be added
# students can be flagged as inactive or active - but not deleted
# students have course sections they are associated with
# ######## multiple student records could exist if a student is in multiple courses

# Instructors
# instructors are selected through an interface at login
# passwords are needed 
# direct web access is possible  
# cookies or another secure method is needed for admin tasks
# instructors need to be added by an admin

##################

#Exercises
# exercises are selected by instructors when creating lessons
# exercises and be ordered and associated with specific dates
# exercises are associated with grade records
# exercises are small - single events

#



