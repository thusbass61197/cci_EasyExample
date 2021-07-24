# Edit following two lines to set component requirements (see docs)


set(COMPONENT_SRCS 
  "AddOn/AppIso_Output.c"
)

set(COMPONENT_ADD_INCLUDEDIRS 
  "."
  ".."
  "AddOn"
)

set(COMPONENT_REQUIRES 
)

set(COMPONENT_PRIV_REQUIRES 
	lib_cci 
	IsoConfig
)


register_component()

