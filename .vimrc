
function! s:insertIncludeGuard()
    let fullname = toupper(expand('%'))
    let reducedname = substitute(fullname, 'src\/', '', '')
    let gatename = substitute(substitute(reducedname, '\.', '_', 'g'), '\/', '_', 'g')
    execute "normal! i#ifndef TSYM_" . gatename
    execute "normal! o#define TSYM_" . gatename . "\n"
    execute "normal! Go#endif"
    normal! kk
endfunction

autocmd BufNewFile *.{h,hpp} call <SID>insertIncludeGuard()

map <F10> :argadd `git ls-files '*.h' '*.cpp'`

call add(g:UltiSnipsSnippetDirectories, getcwd() . '/doc/ultisnips')

set path=bin/**,doc/**,src/**,test/**

set shellpipe=2>&1\ \|
            \while\ read\ line;
            \do\ mod=`echo\ $line\ \|
            \sed\ -e\ 's/build.src/src/g'\ -e\ 's/build.test/test/g'`;
            \echo\ $mod\ >>\ %s;
            \echo\ $line;
            \done

func! AlternateImplVSplit()
    let currentFile = expand('%:t')

    let baseName = matchstr(currentFile, '.*\.')
    let header = matchstr(currentFile, '\.h')
    let impl =  matchstr(currentFile, '\.cpp')

    if (strlen(header))
        let suffix = 'cpp'
    elseif (strlen(impl))
        let suffix = 'h'
    else
        return
    endif

    let newFile = baseName . suffix

    vsplit
    execute "normal! \<C-W>\<C-W>"

    try
       execute 'find ' . newFile
    catch /E345/
       execute "normal! :q\<CR>"
    endtry
endfu

command! LocalAV call AlternateImplVSplit()
cabbrev av LocalAV
