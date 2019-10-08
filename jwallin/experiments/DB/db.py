import mysql.connector

servername = "localhost"
username = "myuser"
password = "mysqlUsernamePassword"
db = "duckduck"

mydb = mysql.connector.connect(
        host=servername,
        user=username,
        passwd=password,
        auth_plugin="mysql_native_password"
        #database=db
        )

print(mydb)

mycursor = mydb.cursor()
mycursor.execute("CREATE DATABASE "+ db)
print("DDDD\n")
mycursor.execute("SHOW DATABASES")
for x in mycursor:
    print(x)

print("kkkkkkk\n")

mycursor.execute("DROP DATABASE "+ db)



dbname = "myDB"
dbAction = ["SELECT", "CREATE", "DROP"]
taction = 1

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

table = [];

table.append( ["Devices", [
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["macid","VARCHAR(30) NOT NULL"],
        ["ipaddress", "VARCHAR(30) NOT NULL"],
        ["school","INT UNSIGNED"], 
        ["reg_date", "TIMESTAMP"]
        ]])


table.append(["Schools", [
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["name","VARCHAR(100) NOT NULL"],
        ["city", "VARCHAR(50) NOT NULL"],
        ["state","VARCHAR(50)"], 
        ["country","VARCHAR(50)"], 
        ["reg_date", "TIMESTAMP"]
        ]])

table.append(["Courses", [
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["school", "INT UNSIGNED"],
        ["fullname","VARCHAR(100) NOT NULL"],
        ["catalogdesignation", "VARCHAR(50) NOT NULL"],
        ["subject","INT UNSIGNED"]
        ]])

table.append(["Subject",[
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["name","VARCHAR(100) NOT NULL"]
        ]])

table.append(["ClassOfferings",[
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["course", "INT UNSIGNED"],
        ["instructor", "INT UNSIGNED"],
        ["startdate", "TIMESTAMP"],
        ["enddate", "TIMESTAMP"],
        ["schedule", "VARCHAR(100)"],
        ["reg_date", "TIMESTAMP"]
        ]])

table.append(["Students",[
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["schoolid", "VARCHAR (100)"],
        ["lastname","VARCHAR (100) NOT NULL"],
        ["firstname", "VARCHAR (100) NOT NULL"],
        ["reg_date", "TIMESTAMP"]
        ]])

table.append(["Instructors",[
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["schoolid", "VARCHAR (100)"],
        ["lastname","VARCHAR (100) NOT NULL"],
        ["firstname", "VARCHAR (100) NOT NULL"],
        ["reg_date", "TIMESTAMP"]
        ]])



table.append(["Lessons",[
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["name", "VARCHAR (100) NOT NULL"],
        ["description", "VARCHAR (1000) NOT NULL"],
        ["exerciselist",  "VARCHAR (10000) NOT NULL"],
        ["reg_date", "TIMESTAMP"]
        ]])

table.append(["Exercises",[
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["exercise", "VARCHAR (100) NOT NULL"],
        ["description", "VARCHAR (1000) NOT NULL"],
        ["lastname","VARCHAR (100) NOT NULL"],
        ["firstname", "VARCHAR (100) NOT NULL"],
        ["reg_date", "TIMESTAMP"],
        ]])

table.append(["ExerciseElements",[
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["exercise","INT"],
        ["elementnumber","INT UNSIGNED"],
        ["elementtype", "INT UNSIGNED"],
        ["timestart", "INT"],
        ["reg_date", "TIMESTAMP"],
        ]])

table.append(["Grades",[
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["lesson","INT UNSIGNED"],
        ["exercise","INT UNSIGNED"],
        ["evaluation","INT UNSIGNED"],
        ["score", "INT UNSIGNED"],
        ["reg_date", "TIMESTAMP"]
        ]])

table.append(["Objects",[
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["name", "VARCHAR (100) NOT NULL"],
        ["object","JSON"],
        ["reg_date", "TIMESTAMP"]
        ]])

table.append(["Evaluations", [
        ["id","INT UNSIGNED AUTO_INCREMENT PRIMARY KEY"],
        ["maxscore", "INT UNSIGNED"],
        ["reg_date", "TIMESTAMP"]
        ]])

# mycursor.execute("CREATE TABLE customers (name VARCHAR(255), address VARCHAR(255))")

# construct the SQL to make the tables
lt = len(table)
for i in range(lt):
    tname = table[i][0]
    sql = dbAction[taction] + " TABLE " + tname + " (\n"
    ncol = len(table[i][1])
    for j in range(ncol):
        column = table[i][1][j]
        cname = column[0]
        cdesc = column[1]
        if j < ncol-1:
            sql = sql + cname + " " + cdesc + ",\n"
        else:
            sql = sql + cname + " " + cdesc + "\n"
    sql = sql + ")"
    print (sql)
    print ("")

print ("---------------")
taction =2
lt = len(table)
for i in range(lt):
    tname = table[i][0]
    sql = dbAction[taction] + " TABLE " + tname 
    print (sql)
    print ("")

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



