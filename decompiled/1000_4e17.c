
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far tzset(void)

{
  char *pcVar1;
  uint uVar2;
  undefined2 uVar3;
  int iVar4;
  char *s;
  undefined4 uVar5;
  
  s = getenv((char *)0x41253f0e);
  uVar3 = (undefined2)((ulong)s >> 0x10);
  pcVar1 = (char *)s;
  if (((((s == (char *)0x0) || (uVar2 = strlen(s), uVar2 < 4)) ||
       ((*(byte *)(*s + 0x3a5f) & 0xc) == 0)) ||
      ((((*(byte *)(pcVar1[1] + 0x3a5f) & 0xc) == 0 || ((*(byte *)(pcVar1[2] + 0x3a5f) & 0xc) == 0))
       || ((pcVar1[3] != '-' && ((pcVar1[3] != '+' && ((*(byte *)(pcVar1[3] + 0x3a5f) & 2) == 0)))))
       ))) || (((*(byte *)(pcVar1[3] + 0x3a5f) & 2) == 0 &&
               ((*(byte *)(pcVar1[4] + 0x3a5f) & 2) == 0)))) {
    DAT_4125_3f0c = 1;
    DAT_4125_3f0a = 0;
    DAT_4125_3f08 = 18000;
    strcpy(_DAT_4125_3f00,(char *)0x41253f11);
    strcpy(_DAT_4125_3f04,(char *)0x41253f15);
    uVar5 = CONCAT22(DAT_4125_3f0a,DAT_4125_3f08);
  }
  else {
    memset(_DAT_4125_3f04,0,4);
    strncpy(_DAT_4125_3f00,s,3);
    ((char *)_DAT_4125_3f00)[3] = '\0';
    atol((char *)CONCAT22(uVar3,pcVar1 + 3));
    uVar5 = __N_LXMUL();
    DAT_4125_3f0a = (undefined2)((ulong)uVar5 >> 0x10);
    DAT_4125_3f08 = (undefined2)uVar5;
    DAT_4125_3f0c = 0;
    for (iVar4 = 3; pcVar1[iVar4] != '\0'; iVar4 = iVar4 + 1) {
      if ((*(byte *)(pcVar1[iVar4] + 0x3a5f) & 0xc) != 0) {
        uVar2 = strlen((char *)CONCAT22(uVar3,pcVar1 + iVar4));
        uVar5 = CONCAT22(DAT_4125_3f0a,DAT_4125_3f08);
        if (((2 < uVar2) &&
            (uVar5 = CONCAT22(DAT_4125_3f0a,DAT_4125_3f08),
            (*(byte *)(pcVar1[iVar4 + 1] + 0x3a5f) & 0xc) != 0)) &&
           (uVar5 = CONCAT22(DAT_4125_3f0a,DAT_4125_3f08),
           (*(byte *)(pcVar1[iVar4 + 2] + 0x3a5f) & 0xc) != 0)) {
          strncpy(_DAT_4125_3f04,(char *)CONCAT22(uVar3,pcVar1 + iVar4),3);
          ((char *)_DAT_4125_3f04)[3] = '\0';
          uVar5 = CONCAT22(DAT_4125_3f0a,DAT_4125_3f08);
          DAT_4125_3f0c = 1;
        }
        break;
      }
    }
  }
  DAT_4125_3f0a = (undefined2)((ulong)uVar5 >> 0x10);
  DAT_4125_3f08 = (undefined2)uVar5;
  return;
}


