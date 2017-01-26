// Generated by gencpp from file lyf_playground/FloatArray.msg
// DO NOT EDIT!


#ifndef LYF_PLAYGROUND_MESSAGE_FLOATARRAY_H
#define LYF_PLAYGROUND_MESSAGE_FLOATARRAY_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>


namespace lyf_playground
{
template <class ContainerAllocator>
struct FloatArray_
{
  typedef FloatArray_<ContainerAllocator> Type;

  FloatArray_()
    : data()  {
    }
  FloatArray_(const ContainerAllocator& _alloc)
    : data(_alloc)  {
  (void)_alloc;
    }



   typedef std::vector<float, typename ContainerAllocator::template rebind<float>::other >  _data_type;
  _data_type data;




  typedef boost::shared_ptr< ::lyf_playground::FloatArray_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::lyf_playground::FloatArray_<ContainerAllocator> const> ConstPtr;

}; // struct FloatArray_

typedef ::lyf_playground::FloatArray_<std::allocator<void> > FloatArray;

typedef boost::shared_ptr< ::lyf_playground::FloatArray > FloatArrayPtr;
typedef boost::shared_ptr< ::lyf_playground::FloatArray const> FloatArrayConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::lyf_playground::FloatArray_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::lyf_playground::FloatArray_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace lyf_playground

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': False, 'IsMessage': True, 'HasHeader': False}
// {'std_msgs': ['/opt/ros/kinetic/share/std_msgs/cmake/../msg'], 'lyf_playground': ['/home/ubuntu/drones/src/lyf_playground/msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::lyf_playground::FloatArray_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::lyf_playground::FloatArray_<ContainerAllocator> const>
  : FalseType
  { };

template <class ContainerAllocator>
struct IsMessage< ::lyf_playground::FloatArray_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::lyf_playground::FloatArray_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::lyf_playground::FloatArray_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::lyf_playground::FloatArray_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::lyf_playground::FloatArray_<ContainerAllocator> >
{
  static const char* value()
  {
    return "420cd38b6b071cd49f2970c3e2cee511";
  }

  static const char* value(const ::lyf_playground::FloatArray_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x420cd38b6b071cd4ULL;
  static const uint64_t static_value2 = 0x9f2970c3e2cee511ULL;
};

template<class ContainerAllocator>
struct DataType< ::lyf_playground::FloatArray_<ContainerAllocator> >
{
  static const char* value()
  {
    return "lyf_playground/FloatArray";
  }

  static const char* value(const ::lyf_playground::FloatArray_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::lyf_playground::FloatArray_<ContainerAllocator> >
{
  static const char* value()
  {
    return "float32[] data\n\
";
  }

  static const char* value(const ::lyf_playground::FloatArray_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::lyf_playground::FloatArray_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.data);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER;
  }; // struct FloatArray_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::lyf_playground::FloatArray_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::lyf_playground::FloatArray_<ContainerAllocator>& v)
  {
    s << indent << "data[]" << std::endl;
    for (size_t i = 0; i < v.data.size(); ++i)
    {
      s << indent << "  data[" << i << "]: ";
      Printer<float>::stream(s, indent + "  ", v.data[i]);
    }
  }
};

} // namespace message_operations
} // namespace ros

#endif // LYF_PLAYGROUND_MESSAGE_FLOATARRAY_H