func4(edi, esi, edx):
    eax = edx
    eax -= esi
    ecx = eax
    ecx >> 31 //logical right shift
    eax += ecx
    eax >> 1 //arithmatic right shift
    ecx = rax + rsi
    if(edi >= ecx):
        eax = 0
        if(edi <= ecx): return
        esi = rcx + 1
        func4(edi, esi, edx)
        eax = 2 * rax + 1
        return
    if(edi < ecx):
        edx = rcx - 1
        func4(edi, esi, edx)
        eax = 2 * eax
        return