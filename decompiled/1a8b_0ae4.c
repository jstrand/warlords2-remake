
int __cdecl16far direction_between(int x1,int y1,int x2,int y2)

{
  if ((x1 == x2) && (y1 == y2)) {
    return 0xff;
  }
  if (x1 == x2) {
    if (y2 < y1) {
      return 0;
    }
    return 4;
  }
  if (y1 == y2) {
    if (x2 < x1) {
      return 6;
    }
    return 2;
  }
  if (x1 <= x2) {
    if (y2 < y1) {
      return 1;
    }
    return 3;
  }
  if (y2 < y1) {
    return 7;
  }
  return 5;
}


