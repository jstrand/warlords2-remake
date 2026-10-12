
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_6f8c_076d(void)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 in_DX;
  
  if (_LAB_4125_0e4b_1 == 0) {
    uVar1 = mem_alloc(0x321,0,0x156);
    _LAB_4125_0e4b_1 = CONCAT22(in_DX,uVar1);
    iVar2 = mem_lock(uVar1,in_DX);
    DAT_451b_0504 = CONCAT22(in_DX,iVar2);
    *(undefined1 *)(iVar2 + 800) = 0;
    mem_unlock(_LAB_4125_0e4b_1);
  }
  uVar1 = mem_lock(_LAB_4125_0e4b_1);
  DAT_451b_0504 = CONCAT22(in_DX,uVar1);
  return;
}


