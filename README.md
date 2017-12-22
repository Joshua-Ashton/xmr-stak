Basically just XMR-Stak with:

- No stdout spam
- No logging
- No HTML report

Compiler changes:
- Whole Program Optimization + LTCG /GL + /LTCG
- Favour fast code /Ot
- Auto intrinsics (prob won't do much) /Oi + AVE2 for those (/arch:AVX2)
- No stack checks /GS-

Just use it like xmr-stak.
Already using regular xmr-stak? Just copy our stuff over theirs and it should work.