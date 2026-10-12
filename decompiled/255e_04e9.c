
int __cdecl16far sample_load(char *param_1,undefined2 param_2)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  char *pcVar5;
  uint unaff_DI;
  int iVar6;
  long lVar7;
  long lVar8;
  undefined2 local_8;
  
  if (DAT_4125_37a2 == '\0') {
    return -1;
  }
  if (DAT_4125_37ab != '\0') {
    return 0;
  }
  iVar3 = 0;
  pcVar5 = (char *)&DAT_451b_3108;
  do {
    iVar6 = iVar3;
    if (*pcVar5 == '\0') break;
    pcVar5 = pcVar5 + 0xb;
    iVar3 = iVar3 + 1;
    iVar6 = -1;
  } while (pcVar5 != (char *)0x70d6);
  if (-1 < iVar6) {
    iVar3 = open((char *)CONCAT22(param_2,param_1),2);
    if (iVar3 < 0) {
      return -1;
    }
    lVar7 = filelength(iVar3);
    lVar8 = lVar7;
    uVar1 = mem_alloc(lVar7,0,0xd,0);
    uVar4 = (undefined2)((ulong)((long)iVar6 * 0xb) >> 0x10);
    iVar2 = (int)((long)iVar6 * 0xb);
    *(undefined2 *)((int)(undefined2 *)&DAT_451b_3111 + iVar2) = (int)((ulong)lVar8 >> 0x10);
    *(undefined2 *)((int)(undefined2 *)&DAT_451b_310f + iVar2) = uVar1;
    uVar1 = mem_lock(*(undefined2 *)((int)(undefined2 *)&DAT_451b_310f + iVar2),
                     *(undefined2 *)((int)(undefined2 *)&DAT_451b_3111 + iVar2));
    local_8 = (undefined2)lVar7;
    iVar2 = iVar6 * 0xb;
    *(undefined2 *)((int)(undefined2 *)&DAT_451b_310d + iVar2) = uVar4;
    *(undefined2 *)((int)(undefined2 *)&DAT_451b_310b + iVar2) = uVar1;
    ((undefined1 *)&DAT_451b_3108)[iVar2] = 1;
    *(undefined2 *)((int)(undefined2 *)&DAT_451b_3109 + iVar2) = local_8;
    read(iVar3,(void *)CONCAT22(local_8,(void *)*(undefined2 *)
                                                 ((int)(undefined2 *)&DAT_451b_310d + iVar2)),
         unaff_DI);
    DAT_451b_3106 = 11000;
    DAT_451b_3104 = 0x4125;
    DAT_451b_3102 = 0x704e;
    iVar2 = iVar6 * 0xb;
    DAT_451b_3100 = *(undefined2 *)((int)(undefined2 *)&DAT_451b_3109 + iVar2);
    DAT_451b_30fe = *(undefined2 *)((int)(undefined2 *)&DAT_451b_310d + iVar2);
    DAT_451b_30fc = *(undefined2 *)((int)(undefined2 *)&DAT_451b_310b + iVar2);
    digpak_massage_audio((undefined2 *)&DAT_451b_30fc,0x4125);
    close(iVar3);
  }
  return iVar6;
}


