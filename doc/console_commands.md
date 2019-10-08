# Protocol description
## Used libraries
https://github.com/python-cmd2/cmd2
## Motivation
To interact simple with aff3ct

## Discription
[] optional flag
# Examples

## General
### Set mode

## Data transfer
### Transfer matrix (Client to server) 
(Client):$ push G 4,3 G00, G01,G02...

S: ok  ->remote G is updated   

### Transfer vector (client to server)
(Client): push X 3   G1, G2,G3.

(Server): ok            ->remote X is updated
   
### Transfer vector (server to client )
(Client): pull G

(Server): ok         >local G is updated
    
### Transfer vector (server to client )
(Client): pull X

(Server): ok         >local X is updated
    

## FUNCTION CALL
(Client): encode X Y
(Server): ok

(Client): encode X
(Server): * print * 

(Client): HISO LLR res
(Server): ok

(Client): SIHO LLR 
(Server): * Print *

## local settings
C: local print with zeros
C: local print with dots

## local dumps
X
Y

    



 

