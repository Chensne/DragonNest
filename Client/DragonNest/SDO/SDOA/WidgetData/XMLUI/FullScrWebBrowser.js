f u n c t i o n   O n B r o w s e r R e s i z e ( S e n d e r )  
 {  
     f o r m C a p t i o n . W i d t h   =   S e n d e r . W i d t h ;  
     b t n F o r m C l o s e . l e f t   =   W e b B r o w s e r . W i d t h   -   b t n F o r m C l o s e . W i d t h   -   8 ;  
 }  
  
 f u n c t i o n   O n B r o w s e r T i t l e C h a n g e ( S e n d e r )  
 {  
     v a r   B r o w s e r C a p t i o n   =   S e n d e r . C o n t e n t T i t l e ;  
      
     i f   ( B r o w s e r C a p t i o n   = = " "   | |   B r o w s e r C a p t i o n   = =   " a b o u t : b l a n k " )  
     {  
     	 B r o w s e r C a p t i o n   =   S y s t e m . W i d g e t . N a m e ;  
  
     	 i f   ( ( B r o w s e r C a p t i o n   = =   " A p p l i c a t i o n " )   | |   ( B r o w s e r C a p t i o n   = =   " < A p p l i c a t i o n > " ) )  
 	 	 B r o w s e r C a p t i o n   =   " Q u a n q u a n " ;  
     }  
  
     i f   ( ( S e n d e r . C o n t e n t P r o g r e s s > = 0 )   & &   ( S e n d e r . C o n t e n t P r o g r e s s M a x > 0 )   & &   ( S e n d e r . C o n t e n t P r o g r e s s   <   S e n d e r . C o n t e n t P r o g r e s s M a x ) )  
     {  
 	     B r o w s e r C a p t i o n   =   B r o w s e r C a p t i o n   +   "   ( L o a d i n g . . . "   +   M a t h . f l o o r ( S e n d e r . C o n t e n t P r o g r e s s * 1 0 0 / S e n d e r . C o n t e n t P r o g r e s s M a x )   +   " % ) " ;  
     }  
     f o r m C a p t i o n . C a p t i o n   =   B r o w s e r C a p t i o n ;  
 }  
  
 f u n c t i o n   O n B r o w s e r S t a t u s T e x t C h a n g e ( S e n d e r )  
 {  
     f o r m C a p t i o n . C a p t i o n   =   S e n d e r . C o n t e n t S t a t u s T e x t ;  
 }  
  
 f u n c t i o n   b t n F o r m C l o s e O n C l i c k ( S e n d e r )  
 {  
 	 / / f o r m C a p t i o n . C a p t i o n   =   " a s d f k j a l k s d j f " ;  
 	 W e b B r o w s e r . c l o s e ( ) ;  
 }  
  
 f u n c t i o n   l o a d M a i n ( b r o w s e r )  
 {  
     W e b B r o w s e r . o n r e s i z e   =   O n B r o w s e r R e s i z e ;  
     W e b B r o w s e r . o n c o n t e n t t i t l e c h a n g e   =   O n B r o w s e r T i t l e C h a n g e ;  
 / /     W e b B r o w s e r . o n c o n t e n t s t a t u s T e x t c h a n g e   =   O n B r o w s e r S t a t u s T e x t C h a n g e ;  
     W e b B r o w s e r . o n c o n t e n t p r o g r e s s c h a n g e   =   O n B r o w s e r T i t l e C h a n g e ;  
  
     O n B r o w s e r R e s i z e ( W e b B r o w s e r ) ;  
 }  
  
 l o a d M a i n ( ) ; 