
void __cdecl16far path_cache_reset(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 in_DX;
  
  DAT_451b_0012._0_2_ = mem_lock(DAT_451b_001e);
  param_1 = param_1 * 0xd6;
  iVar2 = 200;
  DAT_451b_0012._2_2_ = in_DX;
  while (uVar1 = DAT_451b_0012._2_2_, iVar2 != 0) {
    *(undefined1 *)((int)DAT_451b_0012 + param_1 + iVar2 + -1 + 0xe) = 0;
    iVar2 = iVar2 + -1;
  }
  *(undefined2 *)((int)DAT_451b_0012 + param_1) = 0;
  *(undefined2 *)((int)DAT_451b_0012 + param_1 + 2) = 0;
  *(undefined2 *)((int)DAT_451b_0012 + param_1 + 4) = 0;
  *(undefined2 *)((int)DAT_451b_0012 + param_1 + 6) = 0;
  *(undefined2 *)((int)DAT_451b_0012 + param_1 + 8) = 0;
  *(undefined2 *)((int)DAT_451b_0012 + param_1 + 10) = 0;
  *(undefined2 *)((int)DAT_451b_0012 + param_1 + 0xc) = 0;
  mem_unlock(DAT_451b_001e);
  return;
}


