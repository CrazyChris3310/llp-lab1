# llp-1

Lab 1 on low level programming. Implementation of data storage engine of relational databases.

All data about existing tables is stored inside two metadata tables that have the following structure

Table of all tables:

`| name | first_page | record_size | id |`

Table of all columns:

`| name | table_id | offset | length | length | type |`