
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_8611_0ccf(void)

{
  undefined2 uVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined2 in_DX;
  int iVar4;
  undefined1 *local_6;
  
  uVar1 = mem_alloc(0x4440,0,0x29a);
  _LAB_4125_126f_1 = CONCAT22(in_DX,uVar1);
  puVar2 = (undefined1 *)mem_lock(uVar1,in_DX);
  _LAB_4125_126b_1 = (undefined1 *)CONCAT22(in_DX,puVar2);
  local_6 = _LAB_4125_126b_1;
  for (iVar4 = 0; iVar4 < 0x9c; iVar4 = iVar4 + 1) {
    for (iVar3 = 0; iVar3 < 0x70; iVar3 = iVar3 + 1) {
      *local_6 = 0;
      local_6 = (undefined1 *)CONCAT22(local_6._2_2_,(undefined1 *)local_6 + 1);
    }
  }
  mem_unlock(_LAB_4125_126f_1);
  return;
}


