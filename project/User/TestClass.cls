Class User.TestClass
{

ClassMethod MyClassMethod1() [ Language = objectscript ]
{
	Set ^test("MyClassMethod1")=$ZDATETIME($H)
}

ClassMethod MyClassMethod2(s As %String, i As %Integer) As %Integer [ Language = objectscript ]
{
	Set ^test("MyClassMethod2")=$ZDATETIME($H)_";"_s_";"_i
	Return 456
}

ClassMethod MyClassMethod3(s As %String) As %String [ Language = objectscript ]
{
	Set ^test("MyClassMethod3")=$ZDATETIME($H)_";"_s
	Return "ABC"
}
ClassMethod MyClassMethod4(s As %String) As %String [ Language = objectscript ]
{
	Set ^test("MyClassMethod4",1)=$ZDATETIME($H)
	Set ^test("MyClassMethod4",2)=s
#DEFINE ASCIILONGDATASIZE 1000000
	; creates long string and assign it to local variable val
	k val s $P(val,"A",$$$ASCIILONGDATASIZE)="A" 
	Return val
}

}
