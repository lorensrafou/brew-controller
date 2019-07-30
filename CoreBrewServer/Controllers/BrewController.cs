using System;
using CoreBrewServer.Models;
using Microsoft.AspNetCore.Mvc;
using CoreBrewServer.Helpers;
using CoreBrewServer.Entities;
using System.Linq;

namespace CoreBrewServer.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class BrewController : Controller
    {
        private readonly BrewingContext _context;
        public BrewController(BrewingContext context){
            _context = context;
        }

        // GET: api/Brew/
        [HttpGet]
        public IActionResult Get()
        {
            Brew status = Brewing.GetBrewStatus();

            return StatusCode(201, status);
        }

        // POST: api/Brew/
        [HttpPost]
        public IActionResult Post([FromBody] Brew brew)
        {
            if(brew.running == true){
                BrewDay bd = _context.BrewDays
                    .FirstOrDefault(x => x.Id == brew.brewdayId);

                bd.StartTime = DateTime.Now;

                return StatusCode(201, bd);
            }
            return StatusCode(401);
        }
    }

}