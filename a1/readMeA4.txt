What Not Working:
    mesh collision, player still can go through the mesh object
    Bat random search / path finding
    fish path finding

What Working:
    Mesh object attack
    Player attack 
    mesh status saved

How to run the program:
    make to complie and ./a1 to run the game

More about game implement:
    The game will check if the player move from one cube to another, since the player doesn't move one cube at a time(one move as press w, s, d, a once)
    so usually 3 or 4 move will consider as one turn(from one cube to another). This is base on the getviewlocation and getoldviewlocation function plus float
    number rounding. So it is not extramly precision 

    The mesh attack range is 1 cube ahead, 1 cube behing, 1 cube to right, and 1 cube to left. When the plyer enter this range, mesh will attack.
    So teachnilly, the mesh can attack in any angle. The player have some attack range as the mesh object. (mouse left mouse to attack / the player attack message will
    only be display when the mesh object within plyer attack range).

    In the map, blue cube is fish, pink cube is bat, green cube is cactus. When a mesh object is killed, it will disappear from map. 