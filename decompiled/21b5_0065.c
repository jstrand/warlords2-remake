
undefined2 __cdecl16far FUN_21b5_0065(ulong param_1,void *param_2,undefined2 param_3,int param_4)

{
  long lVar1;
  int iVar2;
  undefined2 uVar3;
  
  FUN_21b5_010a();
  lVar1 = DAT_451b_2e42;
  while( true ) {
    if (lVar1 == 0) {
      FUN_21b5_01f4(param_2,param_3,0,0,0);
      return 0;
    }
    uVar3 = (undefined2)((ulong)lVar1 >> 0x10);
    iVar2 = (int)lVar1;
    if ((param_1 & (long)(1 << (*(byte *)*(undefined4 *)(iVar2 + 8) & 0x1f))) != 0) break;
    if (param_4 != 0) {
      FUN_229d_0164();
    }
    lVar1 = *(long *)(iVar2 + 0xc);
  }
  memcpy((void *)CONCAT22(param_3,param_2),
         (void *)CONCAT22(*(undefined2 *)(iVar2 + 10),(void *)*(undefined2 *)(iVar2 + 8)),0x10);
  if (param_4 != 0) {
    FUN_229d_0164();
  }
  return 1;
}


