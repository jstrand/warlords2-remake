
void __cdecl16far FUN_513d_09b3(int *param_1,int param_2)

{
  byte *pbVar1;
  int iVar2;
  byte bVar3;
  char cVar4;
  char cVar5;
  byte *pbVar6;
  int iVar7;
  int iVar8;
  
  iVar8 = *param_1;
  iVar2 = ((int *)param_1)[1];
  if (param_2 < 6) {
    cVar4 = 'b';
    cVar5 = (char)(param_2 << 1);
  }
  else {
    cVar4 = -0x80;
    cVar5 = ((char)param_2 + -6) * '\x02';
  }
  bVar3 = cVar4 + cVar5;
  pbVar6 = (byte *)(iVar2 * 0xe0 + iVar8 * 2);
  *pbVar6 = 0;
  pbVar1 = pbVar6;
  *pbVar1 = *pbVar1 | bVar3;
  iVar7 = iVar2 * 0xe0 + iVar8 * 2;
  *(undefined1 *)(iVar7 + 2) = 0;
  pbVar1 = (byte *)(iVar7 + 2);
  *pbVar1 = *pbVar1 | bVar3 + 1;
  iVar7 = iVar2 * 0xe0 + iVar8 * 2;
  *(undefined1 *)(iVar7 + 0xe0) = 0;
  pbVar1 = (byte *)(iVar7 + 0xe0);
  *pbVar1 = *pbVar1 | bVar3 + 0x10;
  iVar8 = iVar2 * 0xe0 + iVar8 * 2;
  *(undefined1 *)(iVar8 + 0xe2) = 0;
  pbVar1 = (byte *)(iVar8 + 0xe2);
  *pbVar1 = *pbVar1 | bVar3 + 0x11;
  return;
}


