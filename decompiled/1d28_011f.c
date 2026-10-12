
undefined2 * __cdecl16far mem_alloc(ulong param_1,byte param_2,undefined4 param_3)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 *puVar3;
  uint uVar4;
  bool bVar5;
  
  puVar3 = DAT_4125_3276;
  do {
    if ((*(byte *)(puVar3 + 2) & 0x80) == 0) {
      uVar4 = (uint)(param_1 >> 0x10);
      iVar2 = (int)(((ulong)((uVar4 >> 3 & 1) != 0) << 0x10 |
                    ((ulong)((uVar4 >> 2 & 1) != 0) << 0x10 |
                    ((ulong)((uVar4 >> 1 & 1) != 0) << 0x10 |
                    ((ulong)((param_1 & 0x10000) != 0) << 0x10 | param_1 & 0xffff) >> 1) >> 1) >> 1)
                   >> 1);
      DAT_4125_3276 = puVar3;
      if (uVar4 >> 4 == 0) {
        if ((param_1 & 0xf) != 0) {
          iVar2 = iVar2 + 1;
        }
        bVar5 = iVar2 == -1;
        FUN_1d28_0244();
        if (!bVar5) {
          *(undefined2 **)(char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 = puVar3;
          uVar1 = DAT_4125_3274;
          ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] =
               ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] | param_2;
          *puVar3 = 0;
          puVar3[1] = uVar4 + 1;
          *(byte *)(puVar3 + 2) = param_2;
          *(byte *)(puVar3 + 2) = *(byte *)(puVar3 + 2) | 0x80;
          return puVar3;
        }
      }
      goto LAB_1d28_014e;
    }
    puVar3 = (undefined2 *)((int)puVar3 + 5);
    if (DAT_4125_3278 <= puVar3) {
      puVar3 = (undefined2 *)0x0;
    }
  } while (puVar3 != DAT_4125_3276);
  DAT_4125_3280 = 1;
LAB_1d28_014e:
  if (DAT_4125_3284 != (code *)0x0 || DAT_4125_3286 != 0) {
    (*DAT_4125_3284)(0x1d28,(int)param_3,(int)((ulong)param_3 >> 0x10));
  }
  return (undefined2 *)0x0;
}


