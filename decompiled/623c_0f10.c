
void __cdecl16far vector_city_to(int from_city,int to_city)

{
  int iVar1;
  
  iVar1 = city_is_building(from_city);
  if (iVar1 == 0) {
    *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + from_city * 0x41) = 0;
    *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + from_city * 0x41) = 0xffff;
    *(undefined2 *)(from_city * 0x41 + 0x15b2) = 0xffff;
  }
  else {
    if (from_city < 100) {
      *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + from_city * 0x41) =
           *(undefined2 *)(to_city * 0x41 + 0x157d);
      *(undefined2 *)(from_city * 0x41 + 0x15b2) = *(undefined2 *)(to_city * 0x41 + 0x157f);
    }
    else {
      *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + from_city * 0x41) = 0xfffe;
      *(undefined2 *)(from_city * 0x41 + 0x15b2) = 0xfffe;
    }
    *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + from_city * 0x41) = 1;
  }
  return;
}


