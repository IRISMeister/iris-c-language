Class User.Verify
{

ClassMethod CheckResults()
{

	If ^test'="abc" { w "should be abc",! Return }
	If ^test(1)'="12345" { w "should be 12345",! Return }
	Try {
		Set zdh=$ZDTH(^test("MyClassMethod1")) 
	}
	Catch(e) {
		w "should be a valid date time",! Return
	}
	Try {
		Set zdh=$ZDTH($P(^test("MyClassMethod2"),";",1))
	}
	Catch(e) {
		w "should be a valid date time",! Return
	}
	If $P(^test("MyClassMethod2"),";",2)'="my ascii string data" { w "should be my ascii string data",! Return }
	If $P(^test("MyClassMethod2"),";",3)'=100 { w "should be 100",! Return }
	Try {
		Set zdh=$ZDTH($P(^test("MyClassMethod3"),";",1))
	}
	Catch(e) {
		w "should be a valid date time",! Return
	}
	If $P(^test("MyClassMethod3"),";",2)'="my ascii string data" { w "should be my ascii string data",! Return }
	Try {
		Set zdh=$ZDTH(^test("MyClassMethod4",1))
	}
	Catch(e) {
		w "should be a valid date time",! Return
	}
	If ^test("MyClassMethod4",2)'="my ascii string data" { w "should be my ascii string data",! Return }

	For i=1:1:10 {
		If ^test2(i)'="abc" { W "should be abc",! Return }
	}
	Try {
		Set zdh=$ZDTH(^calltest(1))
	}
	Catch(e) {
		w "should be a valid date time",! Return
	}
	Try {
		Set zdh=$ZDTH($P(^calltest(2),";",1))
	}
	Catch(e) {
		w "should be a valid date time",! Return
	}
	If $P(^calltest(2),";",2)'="my ascii string data" { w "should be my ascii string data",! Return }
	If $P(^calltest(2),";",3)'=100 { w "should be 100",! Return }
	If ^execute'=1 { w "should be 1",! Return } 
	Try {
		Set zdh=$ZDTH($P(^execute(1),"/",1,*-1))
	}
	Catch(e) {
		w "should be a valid date time",! Return
	}
	If ^unicode(1)'="あいうえお" { w "should be あいうえお",! Return }
	If $L(^long(1))'=1000000 { w "Length should be 1000000",! Return }
	kill a Set $P(a,"A",1000000)="A"
	If ^long(1)'=a { w "should all A",! Return }
	If $L(^long(2))'=1000000 { w "Length should be 1000000",! Return }
	kill a Set $P(a,"あ",1000000)="あ"
	If ^long(2)'=a { w "should all あ",! Return }

	w "OK",!
}

}
