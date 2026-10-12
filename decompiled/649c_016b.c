
int __cdecl16far city_make_ruins(int x,int y)

{
  byte *pbVar1;
  int iVar2;
  char cVar3;
  int iVar4;
  byte *pbVar5;
  int iVar6;
  int iVar7;
  
  iVar4 = FUN_828e_044a(x,y);
  cVar3 = *(char *)(iVar4 * 0x41 + 0x1592) * '\x02';
  iVar7 = *(int *)(iVar4 * 0x41 + 0x157d);
  iVar2 = *(int *)(iVar4 * 0x41 + 0x157f);
  *(undefined1 *)(iVar4 * 0x41 + 0x1592) = 0xf;
  *(undefined1 *)(iVar4 * 0x41 + 0x15ac) = 0xf;
  pbVar5 = (byte *)(iVar2 * 0xe0 + iVar7 * 2);
  *pbVar5 = 0;
  pbVar1 = pbVar5;
  *pbVar1 = *pbVar1 | cVar3 + 0xa0U;
  iVar6 = iVar2 * 0xe0 + iVar7 * 2;
  *(undefined1 *)(iVar6 + 0xe0) = 0;
  pbVar1 = (byte *)(iVar6 + 0xe0);
  *pbVar1 = *pbVar1 | cVar3 + 0xb0U;
  iVar6 = iVar2 * 0xe0 + iVar7 * 2;
  *(undefined1 *)(iVar6 + 2) = 0;
  pbVar1 = (byte *)(iVar6 + 2);
  *pbVar1 = *pbVar1 | cVar3 + 0xa1U;
  iVar7 = iVar2 * 0xe0 + iVar7 * 2;
  *(undefined1 *)(iVar7 + 0xe2) = 0;
  pbVar1 = (byte *)(iVar7 + 0xe2);
  *pbVar1 = *pbVar1 | cVar3 + 0xb1U;
  if (*(int *)((undefined1 *)&LAB_4125_15ad_1 + iVar4 * 0x41) != 0) {
    *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + iVar4 * 0x41) = 0;
  }
  for (iVar7 = 0; iVar7 < city_count; iVar7 = iVar7 + 1) {
    if (((*(int *)((undefined1 *)&LAB_4125_15ad_1 + iVar7 * 0x41) != 0) &&
        (*(int *)((undefined1 *)&LAB_4125_15b0 + iVar7 * 0x41) == *(int *)(iVar4 * 0x41 + 0x157d)))
       && (*(int *)(iVar7 * 0x41 + 0x15b2) == *(int *)(iVar4 * 0x41 + 0x157f))) {
      *(undefined1 *)(iVar7 * 0x41 + 0x15aa) = 0;
      *(undefined2 *)(iVar7 * 0x41 + 0x15b2) = 0;
      *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + iVar7 * 0x41) = 0;
      *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + iVar7 * 0x41) = 0;
      *(undefined1 *)(iVar7 * 0x41 + 0x15a9) = 0xff;
    }
  }
  return iVar4;
}


