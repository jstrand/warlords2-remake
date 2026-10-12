
void __cdecl16far set_city_tiles(int *city,int owner)

{
  byte *pbVar1;
  int iVar2;
  byte bVar3;
  char cVar4;
  char cVar5;
  byte *pbVar6;
  int iVar7;
  int iVar8;
  
  if (owner == 0xf) {
    owner = -1;
  }
  iVar8 = *city;
  iVar2 = ((int *)city)[1];
  if (owner < 6) {
    cVar4 = 'b';
    cVar5 = (char)(owner << 1);
  }
  else {
    cVar4 = -0x80;
    cVar5 = ((char)owner + -6) * '\x02';
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


