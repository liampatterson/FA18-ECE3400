void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

byte visited[81]; 
StackArray <byte> stack;
byte startNode;
byte current;
void DFSimplementation( void ) {
  
  
  startNode = B0;
  stack.push(startNode);


  /*Helper function to add a new byte to visited[]*/
  void add(byte b){
    for(int k = 1; k< 81; k++)
    {
      if(visited[k] == B0){
        visited[k] = b; //add it to the end of the array
      }
    }
  }

  boolean in(byte b){
    for(i = 0; i<81; i++){
        if(visited[i] == b){
          return true;
        }
    }
    return false;
  }
  
  boolean didSomething = false;
  //not sure how to write possibleForwardNode etc, need to know orientation and coordinate mapping stuff
  while(!stack.empty()){
    current = stack.pop;
    if(in(current)){  //if current is not in visited
      visited.add(current); //set current coordinate to be visited, TODO add an add function
    }
    decodePossibleSets( orientation );
    //check the unvisited nodes first
    switch(caseVariable){
      case B101:
      case B001:
      case B100:
      case B000:
        if(in(possibleForwardNode)){
          Straight();
          didSomething = true;
          foundVertex = false;
          orientRobot( caseVariable );
          transmitSqData( yVal, xVal );
          stack.push(possibleForwardNode);
        }
        break;
      case B001:
      case B011:
      case B010:
        if(in(possibleLeftNode)){
          goLeft();
          didSomething = true;
          foundVertex = false;
          orientRobot( caseVariable );
          transmitSqData( yVal, xVal );
          stack.push(possibleLeftNode);
        }
        break;
      case B100:
      case B110:
        if(in(possibleRightNode)){
          goRight();
          didSomething = true;
          foundVertex = false;
          orientRobot( caseVariable );
          transmitSqData( yVal, xVal );
          stack.push(possibleRightNode);
        }
        break;   
      default:
        break; 
    }
    if(!didSomething){
      //now check everything around you if you have to go to a visited node
      switch(caseVariable){
        case B101:
        case B001:
        case B100:
        case B000:
          Straight();
          foundVertex = false;
          orientRobot( caseVariable );
          transmitSqData( yVal, xVal );
          stack.push(possibleForwardNode);
          break;
        case B001:
        case B011:
        case B010:
          goLeft();
          foundVertex = false;
          orientRobot( caseVariable );
          transmitSqData( yVal, xVal );
          stack.push(possibleLeftNode);
          break;
        case B100:
        case B110:
          goRight();
          foundVertex = false;
          orientRobot( caseVariable );
          transmitSqData( yVal, xVal );
          stack.push(possibleRightNode);
          break;
        case B111:
          turnAround();
          foundVertex = false;
          orientRobot( caseVariable );
          transmitSqData( yVal, xVal );
          break;
        default:
          break;          
      }
    }
  }
}

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
