
void __cdecl16far FUN_6f8c_0a33(void)

{
  char cVar1;
  undefined2 uVar2;
  
  FUN_6f8c_076d();
  cVar1 = *(char *)((int)DAT_451b_0504 + 800);
  FUN_6f8c_07d3();
  if (DAT_451b_0500 < cVar1 + -1) {
    FUN_18a9_05f0(0x100cf);
    uVar2 = 1;
  }
  else {
    FUN_18a9_05f0(0x200cf);
    uVar2 = 2;
  }
  FUN_18a9_05f0(0xd1,uVar2);
  if (DAT_451b_0502 < 1) {
    FUN_18a9_05f0(0x200ce);
    uVar2 = 2;
  }
  else {
    FUN_18a9_05f0(0x100ce);
    uVar2 = 1;
  }
  FUN_18a9_05f0(0xd0,uVar2);
  return;
}


