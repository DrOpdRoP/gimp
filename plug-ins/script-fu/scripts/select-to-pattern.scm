; The GIMP -- an image manipulation program
; Copyright (C) 1995 Spencer Kimball and Peter Mattis
; 
; Based on select-to-brush by
; 	Copyright (c) 1997 Adrian Likins aklikins@eos.ncsu.edu
; Author Cameron Gregory, http://www.flamingtext.com/
;
; Takes the current selection, saves it as a pattern and makes it the active
; pattern
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


(define (script-fu-selection-to-pattern image drawable desc filename)
  (let* (
	 (type (car (gimp-drawable-type drawable)))
	 (old-bg (car (gimp-palette-get-background))))
  
    (set! selection-bounds (gimp-selection-bounds image))
    (set! select-offset-x (cadr selection-bounds))
    (set! select-offset-y (caddr selection-bounds))
    (set! selection-width (- (cadr (cddr selection-bounds)) select-offset-x))
    (set! selection-height (- (caddr (cddr selection-bounds)) select-offset-y))

    (gimp-image-undo-disable image)
    
    (if (= (car (gimp-selection-is-empty image)) TRUE)
	(begin
	  (gimp-selection-layer-alpha drawable)
	  (set! active-selection (car (gimp-selection-save image)))
	  (set! from-selection FALSE))
	(begin
	  (set! from-selection TRUE)
	  (set! active-selection (car (gimp-selection-save image)))))

    (gimp-edit-copy drawable)

    (set! pattern_draw_type RGB-IMAGE)

    (set! pattern_image_type RGB)

    (set! pattern-image (car (gimp-image-new selection-width selection-height pattern_image_type)))

    (set! pattern-draw
          (car (gimp-layer-new pattern-image
                               selection-width
                               selection-height pattern_draw_type "Pattern" 100 NORMAL-MODE)))

    (gimp-image-add-layer pattern-image pattern-draw 0)

    (gimp-selection-none pattern-image)

    (let ((floating-sel (car (gimp-edit-paste pattern-draw FALSE))))
      (gimp-floating-sel-anchor floating-sel))

    
    (set! data-dir (car (gimp-gimprc-query "gimp_dir")))
    (set! filename2 (string-append data-dir
					 "/patterns/"
					 filename
					 (number->string image)
					 ".pat"))

    (file-pat-save 1 pattern-image pattern-draw filename2 "" desc)
    (gimp-patterns-refresh)
    (gimp-patterns-set-pattern desc)

    (gimp-palette-set-background old-bg)

    (gimp-image-undo-enable image)
    (gimp-image-set-active-layer image drawable)
    (gimp-image-delete pattern-image)
    (gimp-displays-flush)))

(script-fu-register "script-fu-selection-to-pattern"
		    _"<Image>/Script-Fu/Selection/To _Pattern..."
		    "Convert a selection to a pattern"
		    "Cameron Gregory <cameron@bloke.com>"
		    "Cameron Gregory"
		    "09/02/2003"
		    "RGB* GRAY*"
		    SF-IMAGE "Image" 0
		    SF-DRAWABLE "Drawable" 0
		    SF-STRING _"Pattern Name" "My Pattern"
		    SF-STRING _"Filename" "mypattern")
