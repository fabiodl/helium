/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

template <typename Device> void Slayer::addComponentType(const helium::HwFactoryName& type){
  if (!factories.insert
      (std::make_pair(type,new helium::HwFactory<Device>(type))
       ).second){
    throw helium::exc::Bug("Two different component factories provide the same type");
  }

}
