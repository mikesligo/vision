if &cp | set nocp | endif
let s:cpo_save=&cpo
set cpo&vim
imap <S-Tab> <Plug>SuperTabBackward
inoremap <C-Tab> 	
vmap [% [%m'gv``
nmap \ihn :IHN
nmap \is :IHS:A
nmap \ih :IHS
nmap <silent> \s :Pytest session
nmap <silent> \m :Pytest method
nmap <silent> \c :Pytest class
nmap <silent> \f :Pytest file
vmap ]% ]%m'gv``
vmap a% [%v]%
nmap gx <Plug>NetrwBrowseX
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#NetrwBrowseX(expand("<cWORD>"),0)
map <silent> <F10> :NERDTreeToggle
map <F8>  
nmap <F7> :TagbarToggle
map <F6> :vsp :exec("tag ".expand("<cword>")))
nnoremap <F5> :buffers:buffer 
map <F4> :A
map <F3> :!make
nnoremap <F2> :set invpaste paste?
imap 	 <Plug>SuperTabForward
imap  <Plug>SuperTabForward
imap  <Plug>SuperTabBackward
imap \ihn :IHN
imap \is :IHS:A
imap \ih :IHS
imap jj 
cmap w!! w !sudo tee % >/dev/null:e!
let &cpo=s:cpo_save
unlet s:cpo_save
set autoindent
set background=dark
set backspace=indent,eol,start
set expandtab
set fileencodings=ucs-bom,utf-8,default,latin1
set formatoptions=tcq1
set helplang=en
set hidden
set ignorecase
set laststatus=2
set nomodeline
set pastetoggle=<F2>
set printoptions=paper:a4
set ruler
set runtimepath=~/.vim,~/.vim/bundle/a,~/.vim/bundle/fugitive,~/.vim/bundle/hybrid,~/.vim/bundle/matchit,~/.vim/bundle/nerdtree,~/.vim/bundle/powerline,~/.vim/bundle/pyflakes,~/.vim/bundle/pytest.vim,~/.vim/bundle/supertab,~/.vim/bundle/tagbar,~/.vim/bundle/vim-flake8,/var/lib/vim/addons,/usr/share/vim/vimfiles,/usr/share/vim/vim74,/usr/share/vim/vimfiles/after,/var/lib/vim/addons/after,~/.vim/after
set shiftwidth=4
set showmatch
set smartcase
set smartindent
set suffixes=.bak,~,.swp,.o,.info,.aux,.log,.dvi,.bbl,.blg,.brf,.cb,.ind,.idx,.ilg,.inx,.out,.toc
set noswapfile
set tabstop=4
set tags=./tags;/
" vim: set ft=vim :
