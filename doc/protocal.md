# Protocol description
## Motivation

## Discription
[] optional flag
# Examples

## General
### Set mode
C: mode TEXT/SHM

S: ok

## Data transfer
### Transfer matrix (Client to server) 
C: set G
    
    4,3
    G00, G01,G02...

S: ok   
### Transfer vector (client to server)
C: set X

    3
    G1, G2,G3.

S: ok

### Transfer vector (server to client )
C: get X

S: 

    4,3
    G00,G01,G02
    

## FUNCTION CALL
C: do encode X -> Y

S: ok
    



 

