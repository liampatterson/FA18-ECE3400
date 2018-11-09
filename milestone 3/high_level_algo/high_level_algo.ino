void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

visited = []; //frontier set

startNode = (0,0);
stack.push(startNode);


//not sure how to write possibleForwardNode etc, need to know orientation and coordinate mapping stuff
while(!stack.empty()){
  curent = stack.pop;
  current.visited = true;
  //unvisited
  if(noFrontWall && possibleForwardNode.visited == false){
    Straight();
    stack.push(possibleForwardNode);
  }
  else if(noLeftWall && possibleLeftNode.visited == false){
    goLeft();
    stack.push(possibleLeftNode);
  }
  else if(noRightWall && possibleRightNode.visited == false){
    goRight();
    stack.push(possibleRightNode);
  }
  else{
    turnAround();
  }
  //has to visit a visited node
  if(noFrontWall){
    Straight();
    stack.push(possibleForwardNode);
  }
  else if(noLeftWall){
    goLeft();
    stack.push(possibleLeftNode);
  }
  else if(noRightWall){
    goRight();
    stack.push(possibleRightNode);
  }
  else{
    turnAround();
  }

}
