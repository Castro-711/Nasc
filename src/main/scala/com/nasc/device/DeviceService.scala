package com.nasc.device

import scala.concurrent.{ExecutionContext, Future}

class DeviceService(implicit executionContext: ExecutionContext) {

  var localDevices = Vector.empty[Device]

  def createDevice(device: Device): Future[Option[String]] = Future {
    localDevices.find(_.mac == device.mac) match {
      case Some(d) => None
      case None =>
        localDevices = localDevices :+ device
        Some(device.mac)
    }
  }

  def getDevice(mac: String): Future[Option[Device]] = Future {
    localDevices.find(_.mac == mac)
  }

  def updateDevice(mac: String, update: DeviceUpdate): Future[Option[Device]] = {

    def updateEntity(device: Device): Device = {
      val rssi = update.rssi.getOrElse(device.rssi).toString.toDouble
      val company = update.company.getOrElse(device.company)
      Device(rssi, mac, company)
    }

    getDevice(mac).flatMap { possibleDevice =>
      possibleDevice match {
        case None => Future { None } // No device found
        case Some(device) =>
          val updateDevice = updateEntity(device)
          deleteDevice(mac).flatMap { _ =>
            createDevice(updateDevice).map(_ => Some(updateDevice))
          }
      }
    }
  }

  def deleteDevice(mac: String): Future[Unit] = Future {
    localDevices = localDevices.filterNot(_.mac == mac)
  }
}


object x extends App {

  implicit val ec = ExecutionContext.global
  val service = new DeviceService

//  val size = service.localDevices.size
//
//  println(s"The number of devices is $size")
}
