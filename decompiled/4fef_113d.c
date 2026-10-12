
void __cdecl16far auto_file_random_sgn(void)

{
  undefined4 uVar1;
  bool bVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int y;
  int iVar6;
  int iVar7;
  void *in_DX;
  int iVar8;
  uint unaff_DI;
  undefined2 uVar9;
  char *name;
  int local_14;
  undefined4 local_6;
  
  local_14 = 0;
  iVar3 = dice(1,0x1e,0x28);
  piVar4 = (int *)FUN_1d28_01d5(iVar3 * 0x68 + 2);
  local_6 = (int *)CONCAT22(in_DX,piVar4);
  *local_6 = iVar3;
  iVar8 = 0;
  do {
    if (iVar3 <= iVar8) {
      iVar8 = 9;
      name = get_file_string(0x21,0);
      iVar8 = open(name,iVar8);
      if (-1 < iVar8) {
        write(iVar8,(void *)CONCAT22(iVar3 * 0x68 + 2,in_DX),unaff_DI);
        close(iVar8);
      }
      FUN_1d28_050d(piVar4,in_DX);
      return;
    }
    bVar2 = false;
    do {
      iVar5 = dice(1,0x70,-1);
      y = dice(1,0x9c,-1);
      if ((((*(char *)(*(byte *)(y * 0xe0 + iVar5 * 2) + 0x710) == '\a') &&
           (iVar6 = terrain_near(iVar5,y,10), iVar6 == 0)) &&
          (iVar6 = terrain_near(iVar5,y,9), iVar6 == 0)) &&
         ((*(byte *)(y * 0x70 + iVar5) & 0x1f) == 0)) {
        bVar2 = true;
      }
    } while (!bVar2);
    *(undefined1 *)(y * 0xe0 + iVar5 * 2) = 0;
    piVar4[iVar8 * 0x34 + 1] = iVar5;
    piVar4[iVar8 * 0x34 + 2] = y;
    iVar6 = dice(1,10,0);
    if (iVar6 < 4) {
      uVar9 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      if (*(int *)((int)DAT_451b_002a + 0x18b0) <= local_14) goto LAB_4fef_12ea;
      strcpy((char *)CONCAT22(in_DX,piVar4 + iVar8 * 0x34 + 3),
             (char *)CONCAT22(uVar9,(char *)((int)DAT_451b_002a + local_14 * 0x3c + 0x18b2)));
      strcpy((char *)CONCAT22(in_DX,piVar4 + iVar8 * 0x34 + 0x1c),
             (char *)CONCAT22(DAT_451b_002a._2_2_,
                              (char *)((int)DAT_451b_002a + local_14 * 0x3c + 0x18d0)));
      local_14 = local_14 + 1;
    }
    else {
LAB_4fef_12ea:
      iVar6 = FUN_828e_04fa(iVar5,y,0xe,0);
      iVar7 = map_distance(iVar5,y,*(int *)(iVar6 * 0x41 + 0x157d),*(int *)(iVar6 * 0x41 + 0x157f));
      iVar5 = FUN_828e_0b51(iVar5,y,*(undefined2 *)(iVar6 * 0x41 + 0x157d),
                            *(undefined2 *)(iVar6 * 0x41 + 0x157f));
      sprintf((char *)CONCAT22(in_DX,piVar4 + iVar8 * 0x34 + 3),
              (char *)CONCAT22(DAT_451b_002a._2_2_,(char *)((int)DAT_451b_002a + 0x1888)),
              iVar6 * 0x41 + 0x1581,0x3c04);
      uVar1 = *(undefined4 *)(iVar5 * 4 + 0x2b7c);
      sprintf((char *)CONCAT22(in_DX,piVar4 + iVar8 * 0x34 + 0x1c),
              (char *)CONCAT22(DAT_451b_002a._2_2_,(char *)((int)DAT_451b_002a + 0x189c)),iVar7 << 1
              ,(int)uVar1,(int)((ulong)uVar1 >> 0x10));
    }
    iVar8 = iVar8 + 1;
  } while( true );
}


