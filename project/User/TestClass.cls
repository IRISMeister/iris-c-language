Class User.TestClass
{

ClassMethod MyClassMethod1() [ Language = objectscript ]
{
	Set ^test=$ZDATETIME($H)
}

ClassMethod MyClassMethod2(s As %String, i As %Integer) As %Integer [ Language = objectscript ]
{
	Set ^test=$ZDATETIME($H)_";"_s_";"_i
	Return 456
}

}
