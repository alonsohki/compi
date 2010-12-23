//
// Gramática original
//
DEFINE_RULE(disyuncion,
            FIRST(
                    ( RESERVED, "not" ),
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 ),
            NEXT(
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    RULE ( conjuncion , c )();
    RULE ( disyuncion_prima , d );
    {
        d.hnombre = c.nombre;
        d.htipo = c.tipo;
        d.hgfalse = c.gfalse;
        d.hgtrue = c.gtrue;
        d.hliteral = c.literal;
    }
    d();
    {
        THIS.nombre = d.nombre;
        THIS.tipo = d.tipo;
        THIS.gfalse = d.gfalse;
        THIS.gtrue = d.gtrue;
        THIS.literal = d.literal;
    }
}

//
// Código generado tras expandir las macros.
//
const CTokenizer::SToken disyuncion::ms_firstToken [] =
{
	CTokenizer::SToken ( CTokenizer::RESERVED , "not" ),
	CTokenizer::SToken ( CTokenizer:: OPERATOR , "-" ),
	CTokenizer::SToken ( CTokenizer:: IDENTIFIER ) ,
	CTokenizer::SToken ( CTokenizer:: INTEGER ) ,
	CTokenizer::SToken ( CTokenizer:: REAL ) ,
	CTokenizer::SToken ( CTokenizer:: RESERVED , "true" ) ,
	CTokenizer::SToken ( CTokenizer:: RESERVED ,"false" ) ,
	CTokenizer::SToken ( CTokenizer:: SEPARATOR ,"(" ) ,
	CTokenizer::SToken ( CTokenizer::UNKNOWN, "" )
};
const CTokenizer::SToken disyuncion::ms_nextToken [] =
{
	CTokenizer::SToken ( CTokenizer:: RESERVED , "entonces" ),
	CTokenizer::SToken ( CTokenizer:: RESERVED ,"fin" ) ,
	Tokenizer::SToken ( CTokenizer:: SEPARATOR ,";" ) ,
	CTokenizer::SToken ( CTokenizer:: SEPARATOR ,")" ) ,
	CTokenizer::SToken ( CTokenizer:: SEPARATOR ,"]" ) ,
	CTokenizer::SToken ( CTokenizer:: SEPARATOR ,"," ) ,
	CTokenizer::SToken ( CTokenizer::UNKNOWN, "" )
};

void disyuncion::Execute ()
{
    conjuncion c( this->GetTranslator(), this );
	c();
    disyuncion_prima d( this->GetTranslator(), this );
    {
        d.hnombre = c.nombre;
        d.htipo = c.tipo;
        d.hgfalse = c.gfalse;
        d.hgtrue = c.gtrue;
        d.hliteral = c.literal;
    }
    d();
    {
        (*this).nombre = d.nombre;
        (*this).tipo = d.tipo;
        (*this).gfalse = d.gfalse;
        (*this).gtrue = d.gtrue;
        (*this).literal = d.literal;
    }
}
