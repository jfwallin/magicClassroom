import mysql.connector

mydb = mysql.connector.connect(
  host="localhost",
  user="myuser",
  passwd="pwd123"
)

print(mydb)
