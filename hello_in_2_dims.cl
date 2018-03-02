__kernel void hello(__global char* string)
{
  int x = get_global_id(0);
  int y = get_global_id(1);
  if (y == 0) {
    if (x == 0) {
      string[0] = 'H';
      string[1] = 'e';
      string[2] = 'l';
      string[3] = 'l';
    } else {
      string[4] = 'o';
      string[5] = ',';
      string[6] = ' ';
      string[7] = 'W';
    }
  } else {
    if (x == 0) {
      string[8] = 'o';
      string[9] = 'r';
      string[10] = 'l';
      string[11] = 'd';
    } else {
      string[12] = '!';
      string[13] = '!';
      string[14] = '!';
      string[15] = '\0';
    }
  }
}
