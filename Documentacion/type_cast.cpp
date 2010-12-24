    CString     type_cast               ( const CString& ident,
                                          const CString& from_,
                                          const CString& to_,
                                          CString& gtrue,
                                          CString& gfalse,
                                          const char* szFile,
                                          unsigned int uiLine )
    {
        // Si son ambos iguales, no hacemos conversiones.
        if ( from_ == to_ )
            return ident;

        // Obtenemos la información de tipos.
        CTypeInfo from ( from_ );
        CTypeInfo to ( to_ );

        // Si alguno de ellos es desconocido, no hacemos conversiones.
        if ( from.GetType() == CTypeInfo::UNKNOWN || to.GetType() == CTypeInfo::UNKNOWN )
            return ident;

        switch ( from.GetType() )
        {
            case CTypeInfo::INTEGER:
            {
                switch ( to.GetType() )
                {
                    case CTypeInfo::REAL:
                    {
                        CString newIdent = new_ident ();
                        push_instruction ( newIdent || " := " || ident );
                        push_instruction ( CString("int2real ") || newIdent );
                        return newIdent;
                    }
                    case CTypeInfo::BOOLEAN:
                    {
                        CString newIdent = new_ident ();
                        CString ref = get_ref ();
                        push_instruction ( CString("if ") || ident || " == 0 goto " || CString(ref + 3) );
                        push_instruction ( newIdent || " := true" );
                        push_instruction ( CString("goto ") || CString(ref + 4) );
                        push_instruction ( newIdent || " := false" );
                        return newIdent;
                    }
                    case CTypeInfo::BOOLEXPR:
                    {
                        gfalse = join_lists(gfalse, init_list(get_ref()));
                        push_instruction ( CString("if ") || ident || " == 0 goto " );
                        gtrue = join_lists(gtrue, init_list(get_ref()));
                        push_instruction ( CString("goto ") );
                        return ident;
                    }
                    default: break;
                }
                break;
            }

            case CTypeInfo::REAL:
            {
                switch ( to.GetType() )
                {
                    case CTypeInfo::INTEGER:
                    {
                        CString newIdent = new_ident ();
                        push_instruction ( newIdent || " := " || ident );
                        push_instruction ( CString("real2int ") || newIdent );
                        return newIdent;
                    }
                    case CTypeInfo::BOOLEAN:
                    {
                        CString newIdent = new_ident ();
                        CString ref = get_ref ();
                        push_instruction ( CString("if ") || ident || " == 0.0 goto " || CString(ref + 3) );
                        push_instruction ( newIdent || " := true" );
                        push_instruction ( CString("goto ") || CString(ref + 4) );
                        push_instruction ( newIdent || " := false" );
                        return newIdent;
                    }
                    case CTypeInfo::BOOLEXPR:
                    {
                        gfalse = join_lists(gfalse, init_list(get_ref()));
                        push_instruction ( CString("if ") || ident || " == 0.0 goto " );
                        gtrue = join_lists(gtrue, init_list(get_ref()));
                        push_instruction ( CString("goto ") );
                        return ident;
                    }
                    default: break;
                }
                break;
            }

            case CTypeInfo::BOOLEAN:
            {
                switch ( to.GetType() )
                {
                    case CTypeInfo::BOOLEXPR:
                    {
                        gtrue = join_lists(gtrue, init_list(get_ref()));
                        push_instruction ( CString("if ") || ident || " goto " );
                        gfalse = join_lists(gfalse, init_list(get_ref()));
                        push_instruction ( CString("goto ") );
                        return ident;
                    }
                    default: break;
                }
                break;
            }

            case CTypeInfo::BOOLEXPR:
            {
                switch ( to.GetType() )
                {
                    case CTypeInfo::BOOLEAN:
                    {
                        CString newIdent = new_ident ();
                        CString ref = get_ref ();
                        push_instruction ( newIdent || " := true" );
                        push_instruction ( CString("goto ") || CString(ref + 3) );
                        push_instruction ( newIdent || " := false" );
                        complete ( gtrue, ref );
                        complete ( gfalse, CString(ref + 2) );
                        gtrue = empty_list();
                        gfalse = empty_list();
                        return newIdent;
                    }

                    default: break;
                }
                break;
            }

            default: break;
        }

        error(CString("Cannot convert from ") ||
              CTypeInfo::NameThisType(from.GetType())
              || " to " ||
              CTypeInfo::NameThisType(to.GetType()), szFile, uiLine );
        return ident;
    }
