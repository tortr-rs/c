# turns the one-file c.h into one that can also be only declarations: with C_DECLS_ONLY defined every function
# body and static is left out, so a program with many .c files can include it and link against c.o (c.c)
import re,sys
t=open(sys.argv[1]).read()
n=len(t); i=0; out=[]
def skip_ws(i):
    # copies whitespace, comments and preprocessor lines to out, returns position of the next unit
    global out
    while i<n:
        c=t[i]
        if c in ' \t\r\n': out.append(c); i+=1
        elif t.startswith('/*',i):
            j=t.index('*/',i)+2; out.append(t[i:j]); i=j
        elif t.startswith('//',i):
            j=t.index('\n',i); out.append(t[i:j]); i=j
        elif c=='#' and (i==0 or t[i-1]=='\n'):
            j=i
            while True:
                e=t.find('\n',j)
                if e<0: e=n
                if t[e-1]=='\\' and e<n: j=e+1; continue
                break
            out.append(t[i:e]); i=e
        else: break
    return i
def scan_unit(i):
    # returns (end, kind) kind: 'func' (ends at closing brace) or 'stmt' (ends after ;)
    j=i; paren=0
    while j<n:
        c=t[j]
        if t.startswith('/*',j): j=t.index('*/',j)+2; continue
        if t.startswith('//',j): j=t.index('\n',j); continue
        if c=='"' or c=="'":
            q=c; j+=1
            while t[j]!=q:
                if t[j]=='\\': j+=1
                j+=1
            j+=1; continue
        if c=='(': paren+=1
        elif c==')': paren-=1
        elif c==';' and paren==0: return j+1,'stmt',None
        elif c=='{' and paren==0:
            head=re.sub(r'/\*.*?\*/','',t[i:j],flags=re.S).rstrip()
            isfunc=head.endswith(')') and not head.lstrip().startswith(('typedef','struct','union','enum')) or (head.endswith(')') and '(' in head and not head.startswith(('typedef',)) and not re.match(r'\s*(struct|union|enum)\s+\w+\s*$',head))
            d=0; k=j
            while k<n:
                if t.startswith('/*',k): k=t.index('*/',k)+2; continue
                if t.startswith('//',k): k=t.index('\n',k); continue
                if t[k] in '"\'':
                    q=t[k]; k+=1
                    while t[k]!=q:
                        if t[k]=='\\': k+=1
                        k+=1
                    k+=1; continue
                if t[k]=='{': d+=1
                elif t[k]=='}':
                    d-=1
                    if d==0: k+=1; break
                k+=1
            if isfunc: return k,'func',j
            j=k; continue
        j+=1
    return n,'stmt',None
while True:
    i=skip_ws(i)
    if i>=n: break
    e,kind,brace=scan_unit(i)
    u=t[i:e]
    head=re.sub(r'/\*.*?\*/','',u,flags=re.S).lstrip()
    static=head.startswith('static')
    if '/*nosplit*/' in u:
        out.append(u); i=e; continue
    if kind=='func':
        k=brace-i
        hdr,body=u[:k].rstrip(),u[k:]
        if static: out.append('#ifndef C_DECLS_ONLY\n'+u+'\n#endif')
        else: out.append(hdr+'\n#ifdef C_DECLS_ONLY\n;\n#else\n'+body+'\n#endif')
    else:
        if head.startswith('__asm__'): out.append('#ifndef C_DECLS_ONLY\n'+u+'\n#endif')
        elif head.startswith(('typedef','struct','union','enum','extern')) and not re.search(r'\}\s*\w+(\[.*\])?\s*(=.*)?;$',u,flags=re.S) :
            out.append(u)
        elif static: out.append('#ifndef C_DECLS_ONLY\n'+u+'\n#endif')
        elif head.startswith(('typedef','struct','union','enum')): out.append(u)
        else:
            decl=re.split(r'\s=\s',u,maxsplit=1)[0].rstrip(';').rstrip()
            out.append('#ifdef C_DECLS_ONLY\nextern '+decl+';\n#else\n'+u+'\n#endif')
    i=e
open(sys.argv[2],'w').write(''.join(out))
