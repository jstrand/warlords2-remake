
void __cdecl16far FUN_1ecc_0009(undefined2 param_1,undefined2 param_2)

{
  undefined2 uVar1;
  undefined2 in_DX;
  
  uVar1 = auto_str_palloc_out_of_hdrs();
  DAT_451b_2a3a = CONCAT22(in_DX,uVar1);
  *(undefined4 *)DAT_451b_2a3a = DAT_451b_2a3a;
  *(undefined4 *)((int)DAT_451b_2a3a + 4) = DAT_451b_2a3a;
  *(undefined1 *)((int)DAT_451b_2a3a + 9) = 1;
  *(undefined2 *)((int)DAT_451b_2a3a + 10) = param_1;
  *(undefined2 *)((int)DAT_451b_2a3a + 0xc) = param_2;
  return;
}


