open Osiris
open Xml

type xparent =
	| XMenu of menu
	| XWindow of window
	| XPanel of panel

class type xobj =
  object
	method component : string -> component
	method component : string -> component
	method container : string -> container
	method menu : string -> menu
	method menuitem : string -> menuitem
	method window : string -> window
	method panel : string -> panel
	method button : string -> button
	method listbox : string -> listbox
	method combobox : string -> combobox
	method toggle : string -> toggle
	method radiobutton : string -> radiobutton
	method checkbox : string -> checkbox
	method label : string -> label
	method edit : string -> edit
	method richedit : string -> richedit
	method treeview : string -> treeview

	method process : bool
	method destroy : unit
	method self : component
  end

val null_xobj : xobj

val create : xparent option -> xml -> xobj
val create_in : xobj -> string -> xml -> unit