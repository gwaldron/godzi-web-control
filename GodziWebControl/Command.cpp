#include <GodziWebControl/Command>
#include <GodziWebControl/MapControl>

#include <sstream>

using namespace GodziWebControl;

void Command::setBlock(osg::RefBlock* block)
{
    _block = block;
}

void Command::execute(MapControl* map)
{
    (*this)(map);
    if (_block.valid())
    {
        _block->release();
    }
}

/************************************************************************************************/
CommandQueue::CommandQueue()
{
}

void CommandQueue::add(Command *command)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lk(_commandsMutex);
    _commands.push_back(command);
}

void CommandQueue::execute(MapControl* map)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lk(_commandsMutex);
    for (Commands::iterator itr = _commands.begin(); itr != _commands.end(); ++itr)
    {
        (*itr)->execute(map);
    }
    _commands.clear();
}

/************************************************************************************************/

CommandArguments::CommandArguments( const std::string& json_string )
{
    osgEarth::Json::Reader reader;
    if ( !reader.parse( json_string, _obj ) )
    {
        osg::notify(osg::WARN) << "JSON PARSING ERROR in: " << json_string << std::endl;
    }
    else
    {
        osg::notify(osg::INFO) << "Parsed: " << json_string << std::endl;
    }
}

std::string
CommandArguments::operator [] ( const std::string& key ) const
{
    osgEarth::Json::Value value = _obj.get( key, osgEarth::Json::Value() );

    if ( value.isNull() )
    {
        return "";
    }
    else if ( value.isConvertibleTo(osgEarth::Json::stringValue) )
    {
        return value.asString();
    }
    else
    {
        osgEarth::Json::FastWriter writer;
        return writer.write( value );
    }

    //if ( _obj.isConvertibleTo( osgEarth::Json::stringValue ) )
    //    _obj.get(
    //return _obj.get( key, "" ).asString();
}


//CommandArguments::CommandArguments(const std::string& args)
//{
//    std::stringstream ss(args);
//
//    for (std::string line; std::getline(ss, line);)
//    {
//        std::string::size_type delim = line.find(':');
//        if (delim != std::string::npos)
//        {
//            std::string key = line.substr(0, delim);
//            std::string value = line.substr(delim+1);
//            _map[key] = value;
//            osg::notify(osg::INFO) << "Args[" << key << "] = " << value << std::endl;
//        }
//    }
//}

//const std::string& 
//CommandArguments::operator [] ( const std::string& key ) const
//{
//    static std::string EMPTY_STRING;
//
//    ArgMap::const_iterator i = _map.find( key );
//    return i != _map.end()? i->second : EMPTY_STRING;
//}