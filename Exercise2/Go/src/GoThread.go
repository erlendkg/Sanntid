package main

import (
  "fmt"
  "runtime"
  "time"

)


func goRoutineOne(i chan int){


for k := 0; k < 1000000; k++ {

temp :=  <-i

temp = temp+1

i <-temp

}



}

func goRoutineTwo(i chan int){

  for k := 0; k < 1000000; k++ {

    temp := <-i
    temp = temp-1
    i <-temp

  }



}





func main()  {

  runtime.GOMAXPROCS(runtime.NumCPU())

  i := make(chan int,1)
  i <-0

  go goRoutineOne(i)
  go goRoutineTwo(i)


  time.Sleep(100*time.Millisecond)

  temp := <-i
  fmt.Println(temp)
  close(i)





}
