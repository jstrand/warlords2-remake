
void __cdecl16near FUN_25de_0757(void)

{
  undefined2 in_CX;
  int in_BX;
  undefined2 unaff_SS;
  
  FUN_25de_0763();
  if (in_BX != 0) {
    LOCK();
    *(undefined2 *)(in_BX + 2) = in_CX;
    UNLOCK();
  }
  return;
}


