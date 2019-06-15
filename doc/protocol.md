# Protocol description
## Used libraries
https://github.com/python-cmd2/cmd2
## Motivation
To interruct simple with aff3ct

## Discription
[] optional flag
# Examples

## General
### Set mode
C: mode TEXT/SHM

S: ok

## Data transfer
### Transfer matrix (Client to server) 
C: push G
    
    4,3
    G00, G01,G02...

S: ok   
### Transfer vector (client to server)
C: pull X

    3
    G1, G2,G3.

S: ok

### Transfer vector (server to client )
C: pull X

S: 

    4,3
    G00,G01,G02
    

## FUNCTION CALL
C: do encode X -> Y

S: ok

## local settings
C: local print with zeros
C: local print with dots

## local dumps
X
Y

    



 

