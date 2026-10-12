
long __cdecl16far filelength(int fd)

{
  code *pcVar1;
  undefined2 extraout_DX;
  bool bVar2;
  undefined2 in_stack_00000000;
  undefined2 in_stack_00000002;
  undefined2 uVar3;
  
  uVar3 = 0x4125;
  bVar2 = false;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if (bVar2) {
    in_stack_00000002 = 0;
    in_stack_00000000 = 0;
  }
  else {
    pcVar1 = (code *)swi(0x21);
    uVar3 = (*pcVar1)(extraout_DX,uVar3);
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)(uVar3);
  }
  return CONCAT22(in_stack_00000000,in_stack_00000002);
}


