#!/usr/bin/ruby -w
# encoding: utf-8

require 'sinatra'

set :bind, ARGV[0]
set :port, ARGV[1]

get '/temp' do
  file = File.new("temperature.txt")
  temp = file.gets.to_i
  mtime = file.stat.mtime
  file.close
  return "Temperature: #{temp}°C<br/>Modification time: #{mtime}"
end 
