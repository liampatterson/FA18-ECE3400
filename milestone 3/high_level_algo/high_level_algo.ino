void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

byte visited[81]; 
StackArray <byte> stack;

void DFSimplementation( void ) {
  startNode = (0,0);
  stack.push(startNode);
  
  while(!stack.empty()){
    current = stack.pop;
    if(!visited.contains(current)){  
      visited.add(current); //set current coordinate to be visited
    }
    decodePossibleSets( orientation );
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
      stack.push(possibleBackNode);
    }  }  }

//assumes coordinates are in format char xVal,yVal
void decodePossibleSets( Orientation orientation, byte currentCoords ) {
  //00010001 is coordinate (1,1)
  //00100001 is coordinate (2,1)
  int xVal = ( currentCoords >> 4 )
  int yVal = ( currentCoords && B00001111 )
  
  switch( orientation ) {
    case north_up:
      possibleForwardNode = intToByte( xVal, yVal+1 );
      possibleLeftNode = intToByte( xVal-1, yVal );
      possibleRightNode = intToByte( xVal+1, yVal );
      break;
    case north_left:
      possibleForwardNode = intToByte( xVal+1, yVal );
      possibleLeftNode = intToByte( xVal, yVal+1 );
      possibleRightNode = intToByte( xVal, yVal-1 );
      break;
    case north_back:
      possibleForwardNode = intToByte( xVal, yVal-1 );
      possibleLeftNode = intToByte( xVal+1, yVal );
      possibleRightNode = intToByte( xVal-1, yVal );
      break;
    case north_right:
      possibleForwardNode = intToByte( xVal-1, yVal );
      possibleLeftNode = intToByte( xVal, yVal-1 );
      possibleRightNode = intToByte( xVal, yVal+1 );
      break;
    default:
      break;
  }
}

byte intToByte( int xVal, int yVal ){
  byte returnVal;
  returnVal = ( xVal << 4 ) || ( yVal % 15);
  return returnVal;
}
