package com.nasc.device

import org.junit.runner.RunWith
import org.scalatest.FlatSpec
import org.scalatest.Matchers
import org.scalatest.junit.JUnitRunner

import scala.concurrent.ExecutionContext



@RunWith(classOf[JUnitRunner])
class DeviceServiceTest extends FlatSpec with Matchers {

  implicit val ec = ExecutionContext.global // very important - potential errors without it
  val devService = new DeviceService

  "A device" should "be created when there is no existing device with the same mac address" in {

    val phone = Device(-0.60, "00-14-22-01-23-45", "Workday")

    devService.createDevice(phone).isCompleted

    println(devService.localDevices)
    println(phone)
  }
}
