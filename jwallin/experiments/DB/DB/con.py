import mysql.connector

servername = "localhost"
username = "myuser"
password = "mysqlUsernamePassword"

mydb = mysql.connector.connect(
        host="localhost",
        user="myuser",
        passwd="mysqlUsernamePassword",
        auth_plugin='mysql_native_password')


"""
mydb = mysql.connector.connect(
        host=servername,
        user=username,
        passwd=password)
"""



print(mydb)

